// Copyright (c) 2020 Unfolded, Inc.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include "./line-layer.h"  // NOLINT(build/include)

#include <arrow/builder.h>

#include "./line-layer-fragment.glsl.h"
#include "./line-layer-vertex.glsl.h"
#include "deck.gl/core.h"

using namespace deckgl;
using namespace lumagl;

const std::vector<const Property*> propTypeDefs = {
    new PropertyT<std::string>{
        "widthUnits", [](const JSONObject* props) { return dynamic_cast<const LineLayer::Props*>(props)->widthUnits; },
        [](JSONObject* props, std::string value) { return dynamic_cast<LineLayer::Props*>(props)->widthUnits = value; },
        "pixels"},
    new PropertyT<float>{
        "widthScale", [](const JSONObject* props) { return dynamic_cast<const LineLayer::Props*>(props)->widthScale; },
        [](JSONObject* props, float value) { return dynamic_cast<LineLayer::Props*>(props)->widthScale = value; }, 1.0},
    new PropertyT<float>{
        "widthMinPixels",
        [](const JSONObject* props) { return dynamic_cast<const LineLayer::Props*>(props)->widthMinPixels; },
        [](JSONObject* props, float value) { return dynamic_cast<LineLayer::Props*>(props)->widthMinPixels = value; },
        0.0},
    new PropertyT<float>{
        "widthMaxPixels",
        [](const JSONObject* props) { return dynamic_cast<const LineLayer::Props*>(props)->widthMaxPixels; },
        [](JSONObject* props, float value) { return dynamic_cast<LineLayer::Props*>(props)->widthMaxPixels = value; },
        std::numeric_limits<float>::max()}};

auto LineLayer::Props::getProperties() const -> const Properties* {
  static Properties properties{Properties::from<LineLayer::Props>("LineLayer", propTypeDefs)};
  return &properties;
}

void LineLayer::initializeState() {
  // TODO(ilija@unfolded.ai): Guaranteed to crash when this layer goes out of scope, revisit
  // TODO(ilija@unfolded.ai): Revisit type once double precision is in place
  auto sourcePosition =
      std::make_shared<arrow::Field>("instanceSourcePositions", arrow::fixed_size_list(arrow::float32(), 3));
  auto getSourcePosition = std::bind(&LineLayer::getSourcePositionData, this, std::placeholders::_1);
  this->attributeManager->add(garrow::ColumnBuilder{sourcePosition, getSourcePosition});

  // TODO(ilija@unfolded.ai): Revisit type once double precision is in place
  auto targetPosition =
      std::make_shared<arrow::Field>("instanceTargetPositions", arrow::fixed_size_list(arrow::float32(), 3));
  auto getTargetPosition = std::bind(&LineLayer::getTargetPositionData, this, std::placeholders::_1);
  this->attributeManager->add(garrow::ColumnBuilder{targetPosition, getTargetPosition});

  auto color = std::make_shared<arrow::Field>("instanceColors", arrow::fixed_size_list(arrow::float32(), 4));
  auto getColor = std::bind(&LineLayer::getColorData, this, std::placeholders::_1);
  this->attributeManager->add(garrow::ColumnBuilder{color, getColor});

  auto width = std::make_shared<arrow::Field>("instanceWidths", arrow::float32());
  auto getWidth = std::bind(&LineLayer::getWidthData, this, std::placeholders::_1);
  this->attributeManager->add(garrow::ColumnBuilder{width, getWidth});

  this->models = {this->_getModel(this->context->device)};
  this->_layerUniforms =
      utils::createBuffer(this->context->device, sizeof(LineLayerUniforms), wgpu::BufferUsage::Uniform);
}

void LineLayer::updateState(const Layer::ChangeFlags& changeFlags, const Layer::Props* oldProps) {
  super::updateState(changeFlags, oldProps);

  auto props = std::dynamic_pointer_cast<LineLayer::Props>(this->props());
  float widthMultiplier = props->widthUnits == "pixels" ? this->context->viewport->metersPerPixel() : 1.0;
  LineLayerUniforms layerUniforms;
  layerUniforms.opacity = props->opacity;
  layerUniforms.widthScale = props->widthScale * widthMultiplier;
  layerUniforms.widthMinPixels = props->widthMinPixels;
  layerUniforms.widthMaxPixels = props->widthMaxPixels;

  this->_layerUniforms.SetSubData(0, sizeof(LineLayerUniforms), &layerUniforms);

  /*
  super::updateState(props, oldProps, changeFlags);

  if (changeFlags.extensionsChanged) {
    const {gl} = this->context;
    if (this->state->model) {
      this->state->model;
    }
    this->setState({model: this->_getModel(gl)});
    this->getAttributeManager().invalidateAll();
  }
  */
}

void LineLayer::finalizeState() {}

void LineLayer::drawState(wgpu::RenderPassEncoder pass) {
  // TODO(ilija@unfolded.ai): Remove. updateState currently doesn't seem to be called when viewport changes
  this->updateState(Layer::ChangeFlags{}, nullptr);

  for (auto const& model : this->getModels()) {
    // Layer uniforms are currently bound to index 1
    model->setUniformBuffer(1, this->_layerUniforms);
    model->draw(pass);
  }
}

auto LineLayer::_getModel(wgpu::Device device) -> std::shared_ptr<lumagl::Model> {
  std::vector<std::shared_ptr<garrow::Field>> attributeFields{
      std::make_shared<garrow::Field>("positions", wgpu::VertexFormat::Float3)};
  auto attributeSchema = std::make_shared<lumagl::garrow::Schema>(attributeFields);

  // TODO(ilija@unfolded.ai): **arrow**::Fields are already being specified in initializeState, consolidate?
  std::vector<std::shared_ptr<garrow::Field>> instancedFields{
      std::make_shared<garrow::Field>("instanceSourcePositions", wgpu::VertexFormat::Float3),
      std::make_shared<garrow::Field>("instanceTargetPositions", wgpu::VertexFormat::Float3),
      std::make_shared<garrow::Field>("instanceColors", wgpu::VertexFormat::Float4),
      std::make_shared<garrow::Field>("instanceWidths", wgpu::VertexFormat::Float)};
  auto instancedAttributeSchema = std::make_shared<lumagl::garrow::Schema>(instancedFields);

  auto modelOptions = Model::Options{vs,
                                     fs,
                                     attributeSchema,
                                     instancedAttributeSchema,
                                     {UniformDescriptor{}, UniformDescriptor{}},
                                     wgpu::PrimitiveTopology::TriangleStrip};
  auto model = std::make_shared<lumagl::Model>(device, modelOptions);

  //
  //  (0, -1)-------------_(1, -1)
  //       |          _,-"  |
  //       o      _,-"      o
  //       |  _,-"          |
  //  (0, 1)"-------------(1, 1)
  //
  std::vector<mathgl::Vector3<float>> positionData = {{0, -1, 0}, {0, 1, 0}, {1, -1, 0}, {1, 1, 0}};
  std::vector<std::shared_ptr<garrow::Array>> attributeArrays{
      std::make_shared<garrow::Array>(this->context->device, positionData, wgpu::BufferUsage::Vertex)};
  model->setAttributes(std::make_shared<garrow::Table>(attributeSchema, attributeArrays));

  auto instancedAttributes = this->attributeManager->update(this->props()->data);
  model->setInstancedAttributes(instancedAttributes);

  model->vertexCount = static_cast<int>(positionData.size());

  return model;
}

auto LineLayer::getSourcePositionData(const std::shared_ptr<arrow::Table>& table) -> std::shared_ptr<arrow::Array> {
  auto props = std::dynamic_pointer_cast<LineLayer::Props>(this->props());
  if (!props) {
    throw std::logic_error("Invalid layer properties");
  }

  return ArrowMapper::mapVector3FloatColumn(table, props->getSourcePosition);
}

auto LineLayer::getTargetPositionData(const std::shared_ptr<arrow::Table>& table) -> std::shared_ptr<arrow::Array> {
  auto props = std::dynamic_pointer_cast<LineLayer::Props>(this->props());
  if (!props) {
    throw std::logic_error("Invalid layer properties");
  }

  return ArrowMapper::mapVector3FloatColumn(table, props->getTargetPosition);
}

auto LineLayer::getColorData(const std::shared_ptr<arrow::Table>& table) -> std::shared_ptr<arrow::Array> {
  auto props = std::dynamic_pointer_cast<LineLayer::Props>(this->props());
  if (!props) {
    throw std::logic_error("Invalid layer properties");
  }

  return ArrowMapper::mapVector4FloatColumn(table, props->getColor);
}

auto LineLayer::getWidthData(const std::shared_ptr<arrow::Table>& table) -> std::shared_ptr<arrow::Array> {
  auto props = std::dynamic_pointer_cast<LineLayer::Props>(this->props());
  if (!props) {
    throw std::logic_error("Invalid layer properties");
  }

  return ArrowMapper::mapFloatColumn(table, props->getWidth);
}
