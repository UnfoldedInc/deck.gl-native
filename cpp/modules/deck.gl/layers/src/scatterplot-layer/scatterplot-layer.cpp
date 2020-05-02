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

#include "./scatterplot-layer.h"  // NOLINT(build/include)

#include "./scatterplot-layer-fragment.glsl.h"
#include "./scatterplot-layer-vertex.glsl.h"
#include "deck.gl/core.h"

using namespace deckgl;
using namespace lumagl;

const std::vector<const Property*> propTypeDefs = {
    new PropertyT<bool>{
        "filled", [](const JSONObject* props) { return dynamic_cast<const ScatterplotLayer::Props*>(props)->filled; },
        [](JSONObject* props, bool value) { return dynamic_cast<ScatterplotLayer::Props*>(props)->filled = value; },
        true},
    new PropertyT<bool>{
        "stroked", [](const JSONObject* props) { return dynamic_cast<const ScatterplotLayer::Props*>(props)->stroked; },
        [](JSONObject* props, bool value) { return dynamic_cast<ScatterplotLayer::Props*>(props)->stroked = value; },
        false},
    new PropertyT<std::string>{
        "lineWidthUnits",
        [](const JSONObject* props) { return dynamic_cast<const ScatterplotLayer::Props*>(props)->lineWidthUnits; },
        [](JSONObject* props, std::string value) {
          return dynamic_cast<ScatterplotLayer::Props*>(props)->lineWidthUnits = value;
        },
        "meters"},
    new PropertyT<float>{
        "lineWidthScale",
        [](const JSONObject* props) { return dynamic_cast<const ScatterplotLayer::Props*>(props)->lineWidthScale; },
        [](JSONObject* props, float value) {
          return dynamic_cast<ScatterplotLayer::Props*>(props)->lineWidthScale = value;
        },
        1.0},
    new PropertyT<float>{
        "lineWidthMinPixels",
        [](const JSONObject* props) { return dynamic_cast<const ScatterplotLayer::Props*>(props)->lineWidthMinPixels; },
        [](JSONObject* props, float value) {
          return dynamic_cast<ScatterplotLayer::Props*>(props)->lineWidthMinPixels = value;
        },
        0.0},
    new PropertyT<float>{
        "lineWidthMaxPixels",
        [](const JSONObject* props) { return dynamic_cast<const ScatterplotLayer::Props*>(props)->lineWidthMaxPixels; },
        [](JSONObject* props, float value) {
          return dynamic_cast<ScatterplotLayer::Props*>(props)->lineWidthMaxPixels = value;
        },
        std::numeric_limits<float>::max()},
    new PropertyT<float>{
        "radiusScale",
        [](const JSONObject* props) { return dynamic_cast<const ScatterplotLayer::Props*>(props)->radiusScale; },
        [](JSONObject* props, float value) {
          return dynamic_cast<ScatterplotLayer::Props*>(props)->radiusScale = value;
        },
        1.0},
    new PropertyT<float>{
        "radiusMinPixels",
        [](const JSONObject* props) { return dynamic_cast<const ScatterplotLayer::Props*>(props)->radiusMinPixels; },
        [](JSONObject* props, float value) {
          return dynamic_cast<ScatterplotLayer::Props*>(props)->radiusMinPixels = value;
        },
        0.0},

    new PropertyT<float>{
        "radiusMaxPixels",
        [](const JSONObject* props) { return dynamic_cast<const ScatterplotLayer::Props*>(props)->radiusMaxPixels; },
        [](JSONObject* props, float value) {
          return dynamic_cast<ScatterplotLayer::Props*>(props)->radiusMaxPixels = value;
        },
        std::numeric_limits<float>::max()}};

auto ScatterplotLayer::Props::getProperties() const -> const Properties* {
  static Properties properties{Properties::from<ScatterplotLayer::Props>("ScatterplotLayer", propTypeDefs)};
  return &properties;
}

void ScatterplotLayer::initializeState() {
  // TODO(ilija@unfolded.ai): Guaranteed to crash when this layer goes out of scope, revisit
  // TODO(ilija@unfolded.ai): Revisit type once double precision is in place
  auto position = std::make_shared<arrow::Field>("instancePositions", arrow::fixed_size_list(arrow::float32(), 3));
  auto getPosition = std::bind(&ScatterplotLayer::getPositionData, this, std::placeholders::_1);
  this->attributeManager->add(garrow::ColumnBuilder{position, getPosition});

  auto radius = std::make_shared<arrow::Field>("instanceRadius", arrow::float32());
  auto getRadius = std::bind(&ScatterplotLayer::getRadiusData, this, std::placeholders::_1);
  this->attributeManager->add(garrow::ColumnBuilder{radius, getRadius});

  auto fillColor = std::make_shared<arrow::Field>("instanceFillColors", arrow::fixed_size_list(arrow::float32(), 4));
  auto getFillColor = std::bind(&ScatterplotLayer::getFillColorData, this, std::placeholders::_1);
  this->attributeManager->add(garrow::ColumnBuilder{fillColor, getFillColor});

  auto lineColor = std::make_shared<arrow::Field>("instanceLineColors", arrow::fixed_size_list(arrow::float32(), 4));
  auto getLineColor = std::bind(&ScatterplotLayer::getLineColorData, this, std::placeholders::_1);
  this->attributeManager->add(garrow::ColumnBuilder{lineColor, getLineColor});

  auto lineWidth = std::make_shared<arrow::Field>("instanceLineWidths", arrow::float32());
  auto getLineWidth = std::bind(&ScatterplotLayer::getLineWidthData, this, std::placeholders::_1);
  this->attributeManager->add(garrow::ColumnBuilder{lineWidth, getLineWidth});

  // TODO(ilija@unfolded.ai): Where should we initialize models?
  this->models = {this->_getModel(this->context->device)};
}

void ScatterplotLayer::updateState(const Layer::ChangeFlags& changeFlags, const Layer::Props* oldProps) {
  /*
  super.updateState({props, oldProps, changeFlags});
  if (changeFlags.extensionsChanged) {
    const {gl} = this->context;
    if (this->state.model) {
      this->state.model.delete();
    }
    this->setState({model: this->_getModel(gl)});
    this->getAttributeManager().invalidateAll();
  }
  */
}

void ScatterplotLayer::finalizeState() {}

void ScatterplotLayer::drawState(wgpu::RenderPassEncoder pass) {
  auto props = std::dynamic_pointer_cast<ScatterplotLayer::Props>(this->props());
  float widthMultiplier = props->lineWidthUnits == "pixels" ? this->context->viewport->metersPerPixel() : 1.0;
  auto lineWidthScale = props->lineWidthScale * widthMultiplier;

  ScatterplotLayerUniforms layerUniforms{
      props->opacity, props->radiusScale,        props->radiusMinPixels,    props->radiusMaxPixels,
      lineWidthScale, props->lineWidthMinPixels, props->lineWidthMaxPixels, props->stroked ? 1.0f : 0.0f,
      props->filled};
  for (auto const& model : this->getModels()) {
    // Layer uniforms are currently bound to index 1
    model->setUniforms(
        std::make_shared<garrow::Array>(this->context->device, &layerUniforms, 1, wgpu::BufferUsage::Uniform), 1);
    model->draw(pass);
  }
}

auto ScatterplotLayer::getPositionData(const std::shared_ptr<arrow::Table>& table) -> std::shared_ptr<arrow::Array> {
  auto props = std::dynamic_pointer_cast<ScatterplotLayer::Props>(this->props());
  if (!props) {
    throw std::logic_error("Invalid layer properties");
  }

  return ArrowMapper::mapVector3FloatColumn(table, props->getPosition);
}

auto ScatterplotLayer::getRadiusData(const std::shared_ptr<arrow::Table>& table) -> std::shared_ptr<arrow::Array> {
  auto props = std::dynamic_pointer_cast<ScatterplotLayer::Props>(this->props());
  if (!props) {
    throw std::logic_error("Invalid layer properties");
  }

  return ArrowMapper::mapFloatColumn(table, props->getRadius);
}

auto ScatterplotLayer::getFillColorData(const std::shared_ptr<arrow::Table>& table) -> std::shared_ptr<arrow::Array> {
  auto props = std::dynamic_pointer_cast<ScatterplotLayer::Props>(this->props());
  if (!props) {
    throw std::logic_error("Invalid layer properties");
  }

  return ArrowMapper::mapVector4FloatColumn(table, props->getFillColor);
}

auto ScatterplotLayer::getLineColorData(const std::shared_ptr<arrow::Table>& table) -> std::shared_ptr<arrow::Array> {
  auto props = std::dynamic_pointer_cast<ScatterplotLayer::Props>(this->props());
  if (!props) {
    throw std::logic_error("Invalid layer properties");
  }

  return ArrowMapper::mapVector4FloatColumn(table, props->getLineColor);
}

auto ScatterplotLayer::getLineWidthData(const std::shared_ptr<arrow::Table>& table) -> std::shared_ptr<arrow::Array> {
  auto props = std::dynamic_pointer_cast<ScatterplotLayer::Props>(this->props());
  if (!props) {
    throw std::logic_error("Invalid layer properties");
  }

  return ArrowMapper::mapFloatColumn(table, props->getLineWidth);
}

auto ScatterplotLayer::_getModel(wgpu::Device device) -> std::shared_ptr<lumagl::Model> {
  std::vector<std::shared_ptr<garrow::Field>> attributeFields{
      std::make_shared<garrow::Field>("positions", wgpu::VertexFormat::Float3)};
  auto attributeSchema = std::make_shared<lumagl::garrow::Schema>(attributeFields);

  // TODO(ilija@unfolded.ai): **arrow**::Fields are already being specified in initializeState, consolidate?
  std::vector<std::shared_ptr<garrow::Field>> instancedFields{
      std::make_shared<garrow::Field>("instancePositions", wgpu::VertexFormat::Float3),
      std::make_shared<garrow::Field>("instanceRadius", wgpu::VertexFormat::Float),
      std::make_shared<garrow::Field>("instanceFillColors", wgpu::VertexFormat::Float4),
      std::make_shared<garrow::Field>("instanceLineColors", wgpu::VertexFormat::Float4),
      std::make_shared<garrow::Field>("instanceLineWidths", wgpu::VertexFormat::Float)};
  auto instancedAttributeSchema = std::make_shared<lumagl::garrow::Schema>(instancedFields);

  std::vector<UniformDescriptor> uniforms = {
      {sizeof(ViewportUniforms)},
      {sizeof(ScatterplotLayerUniforms), wgpu::ShaderStage::Vertex | wgpu::ShaderStage::Fragment}};
  auto modelOptions = Model::Options{
      vs, fs, attributeSchema, instancedAttributeSchema, uniforms, wgpu::PrimitiveTopology::TriangleStrip};
  auto model = std::make_shared<lumagl::Model>(device, modelOptions);

  // A square that minimally covers the unit circle
  //
  //  (-1, -1)_------------_(1, -1)
  //        |  "-,_          |
  //        o      "-,_      o
  //        |          "-,_  |
  //  (-1, 1)"-------------"(1, 1)
  //
  std::vector<mathgl::Vector3<float>> positionData = {{-1, 1, 0}, {-1, -1, 0}, {1, 1, 0}, {1, -1, 0}};
  std::vector<std::shared_ptr<garrow::Array>> attributeArrays{
      std::make_shared<garrow::Array>(this->context->device, positionData, wgpu::BufferUsage::Vertex)};
  model->setAttributes(std::make_shared<garrow::Table>(attributeSchema, attributeArrays));

  auto instancedAttributes = this->attributeManager->update(this->props()->data);
  model->setInstancedAttributes(instancedAttributes);

  model->vertexCount = static_cast<int>(positionData.size());

  return model;
}
