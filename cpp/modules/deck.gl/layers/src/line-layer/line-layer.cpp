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
    //  new PropertyT<std::string>{"widthUnits",
    //      [](const JSONObject* props) { return dynamic_cast<const LineLayer*>(props)->widthUnits; },
    //      [](JSONObject* props, bool value) { return dynamic_cast<LineLayer*>(props)->widthUnits = value; },
    //      true},
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

/*
const defaultProps = {
  getSourcePosition: {type: 'accessor', value: x => x.sourcePosition},
  getTargetPosition: {type: 'accessor', value: x => x.targetPosition},
  getColor: {type: 'accessor', value: DEFAULT_COLOR},
  getWidth: {type: 'accessor', value: 1},

  widthUnits: 'pixels',
  widthScale: {type: 'number', value: 1, min: 0},
  widthMinPixels: {type: 'number', value: 0, min: 0},
  widthMaxPixels: {type: 'number', value: Number.MAX_SAFE_INTEGER, min: 0}
};
*/

// getShaders() {
//   return super.getShaders({vs, fs, modules: [project32, picking]});
// }

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

  // TODO(ilija@unfolded.ai): Remove
  this->_model = this->_getModel(this->context->device);
}

void LineLayer::updateState(const Layer::ChangeFlags& changeFlags, const Layer::Props* oldProps) {
  // super::updateState(props, oldProps, changeFlags);

  // if (changeFlags.extensionsChanged) {
  //   const {gl} = this->context;
  //   if (this->state->model) {
  //     this->state->model;
  //   }
  //   this->setState({model: this->_getModel(gl)});
  //   this->getAttributeManager().invalidateAll();
  // }
}

void LineLayer::finalizeState() {}

void LineLayer::_drawState(wgpu::RenderPassEncoder pass) {  // {uniforms}
  auto props = std::dynamic_pointer_cast<LineLayer::Props>(this->props());
  LineLayerUniforms layerUniforms{props->opacity, props->widthScale, props->widthMaxPixels, props->widthMaxPixels};
  // TODO(ilija@unfolded.ai): Calculate uniforms using getUniformsFromViewport or calculateViewportUniforms and set them
  this->_model->setUniforms(
      {std::make_shared<garrow::Array>(this->context->device, &layerUniforms, 1, wgpu::BufferUsage::Uniform)});

  this->_model->draw(pass);
  /*
  const {viewport} = this->context;
  const {widthUnits, widthScale, widthMinPixels, widthMaxPixels} = ;

  const widthMultiplier = widthUnits === 'pixels' ? viewport.metersPerPixel :

  this->state.model
    .setUniforms(
      Object.assign({}, uniforms, {
        widthScale: widthScale * widthMultiplier,
        widthMinPixels,
        widthMaxPixels
      })
    )
    .draw();
  */
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

  // TODO(ilija@unfolded.ai): Get rid of this once shader modules are in place
  std::string combinedVS = std::string{projectVS} + std::string{vs};
  auto modelOptions = Model::Options{combinedVS,
                                     fs,
                                     attributeSchema,
                                     instancedAttributeSchema,
                                     {{sizeof(LineLayerUniforms)}, {sizeof(ViewportUniforms)}}};
  auto model = std::make_shared<lumagl::Model>(device, modelOptions);

  model->vertexCount = 4;  // Is this correct?

  std::vector<mathgl::Vector3<float>> positionData = {{0, -1, 0}, {0, 1, 0}, {1, -1, 0}, {1, 1, 0}};
  std::vector<std::shared_ptr<garrow::Array>> attributeArrays{
      std::make_shared<garrow::Array>(this->context->device, positionData, wgpu::BufferUsage::Vertex)};
  model->setAttributes(std::make_shared<garrow::Table>(attributeSchema, attributeArrays));

  auto instancedAttributes = this->attributeManager->update(this->props()->data);
  model->setInstancedAttributes(instancedAttributes);

  return model;

  //
  //  (0, -1)-------------_(1, -1)
  //       |          _,-"  |
  //       o      _,-"      o
  //       |  _,-"          |
  //  (0, 1)"-------------(1, 1)
  //
  /*
  const positions = [0, -1, 0, 0, 1, 0, 1, -1, 0, 1, 1, 0];

  return new lumagl::Model(
    gl,
    Object.assign({}, this->getShaders(), {
      id: this->props.id,
      geometry: new Geometry({
        drawMode: GL.TRIANGLE_STRIP,
        attributes: {
          positions: new Float32Array(positions)
        }
      }),
      isInstanced: true
    })
  );
  */
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
