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

using namespace deckgl;
using namespace lumagl;

const std::vector<std::shared_ptr<Property>> propTypeDefs = {
    std::make_shared<PropertyT<bool>>(
        "filled", [](const JSONObject* props) { return dynamic_cast<const ScatterplotLayer::Props*>(props)->filled; },
        [](JSONObject* props, bool value) { return dynamic_cast<ScatterplotLayer::Props*>(props)->filled = value; },
        true),
    std::make_shared<PropertyT<bool>>(
        "stroked", [](const JSONObject* props) { return dynamic_cast<const ScatterplotLayer::Props*>(props)->stroked; },
        [](JSONObject* props, bool value) { return dynamic_cast<ScatterplotLayer::Props*>(props)->stroked = value; },
        false),
    std::make_shared<PropertyT<std::string>>(
        "lineWidthUnits",
        [](const JSONObject* props) { return dynamic_cast<const ScatterplotLayer::Props*>(props)->lineWidthUnits; },
        [](JSONObject* props, std::string value) {
          return dynamic_cast<ScatterplotLayer::Props*>(props)->lineWidthUnits = value;
        },
        "meters"),
    std::make_shared<PropertyT<float>>(
        "lineWidthScale",
        [](const JSONObject* props) { return dynamic_cast<const ScatterplotLayer::Props*>(props)->lineWidthScale; },
        [](JSONObject* props, float value) {
          return dynamic_cast<ScatterplotLayer::Props*>(props)->lineWidthScale = value;
        },
        1.0),
    std::make_shared<PropertyT<float>>(
        "lineWidthMinPixels",
        [](const JSONObject* props) { return dynamic_cast<const ScatterplotLayer::Props*>(props)->lineWidthMinPixels; },
        [](JSONObject* props, float value) {
          return dynamic_cast<ScatterplotLayer::Props*>(props)->lineWidthMinPixels = value;
        },
        0.0),
    std::make_shared<PropertyT<float>>(
        "lineWidthMaxPixels",
        [](const JSONObject* props) { return dynamic_cast<const ScatterplotLayer::Props*>(props)->lineWidthMaxPixels; },
        [](JSONObject* props, float value) {
          return dynamic_cast<ScatterplotLayer::Props*>(props)->lineWidthMaxPixels = value;
        },
        std::numeric_limits<float>::max()),
    std::make_shared<PropertyT<float>>(
        "radiusScale",
        [](const JSONObject* props) { return dynamic_cast<const ScatterplotLayer::Props*>(props)->radiusScale; },
        [](JSONObject* props, float value) {
          return dynamic_cast<ScatterplotLayer::Props*>(props)->radiusScale = value;
        },
        1.0),
    std::make_shared<PropertyT<float>>(
        "radiusMinPixels",
        [](const JSONObject* props) { return dynamic_cast<const ScatterplotLayer::Props*>(props)->radiusMinPixels; },
        [](JSONObject* props, float value) {
          return dynamic_cast<ScatterplotLayer::Props*>(props)->radiusMinPixels = value;
        },
        0.0),

    std::make_shared<PropertyT<float>>(
        "radiusMaxPixels",
        [](const JSONObject* props) { return dynamic_cast<const ScatterplotLayer::Props*>(props)->radiusMaxPixels; },
        [](JSONObject* props, float value) {
          return dynamic_cast<ScatterplotLayer::Props*>(props)->radiusMaxPixels = value;
        },
        std::numeric_limits<float>::max())};

auto ScatterplotLayer::Props::getProperties() const -> const std::shared_ptr<Properties> {
  static auto properties = Properties::from<ScatterplotLayer::Props>(propTypeDefs);
  return properties;
}

void ScatterplotLayer::initializeState() {
  // TODO(ilija@unfolded.ai): Revisit type once double precision is in place
  auto position = std::make_shared<arrow::Field>("instancePositions", arrow::fixed_size_list(arrow::float32(), 3));
  auto getPosition = [this](const std::shared_ptr<arrow::Table>& table) { return this->getPositionData(table); };
  this->_attributeManager->add(garrow::ColumnBuilder{position, getPosition});

  auto radius = std::make_shared<arrow::Field>("instanceRadius", arrow::float32());
  auto getRadius = [this](const std::shared_ptr<arrow::Table>& table) { return this->getRadiusData(table); };
  this->_attributeManager->add(garrow::ColumnBuilder{radius, getRadius});

  auto fillColor = std::make_shared<arrow::Field>("instanceFillColors", arrow::fixed_size_list(arrow::float32(), 4));
  auto getFillColor = [this](const std::shared_ptr<arrow::Table>& table) { return this->getFillColorData(table); };
  this->_attributeManager->add(garrow::ColumnBuilder{fillColor, getFillColor});

  auto lineColor = std::make_shared<arrow::Field>("instanceLineColors", arrow::fixed_size_list(arrow::float32(), 4));
  auto getLineColor = [this](const std::shared_ptr<arrow::Table>& table) { return this->getLineColorData(table); };
  this->_attributeManager->add(garrow::ColumnBuilder{lineColor, getLineColor});

  auto lineWidth = std::make_shared<arrow::Field>("instanceLineWidths", arrow::float32());
  auto getLineWidth = [this](const std::shared_ptr<arrow::Table>& table) { return this->getLineWidthData(table); };
  this->_attributeManager->add(garrow::ColumnBuilder{lineWidth, getLineWidth});

  this->_models = {this->_getModel(this->context->device)};
  this->_layerUniforms =
      utils::createBuffer(this->context->device, sizeof(ScatterplotLayerUniforms), wgpu::BufferUsage::Uniform);
}

void ScatterplotLayer::updateState(const Layer::ChangeFlags& changeFlags,
                                   const std::shared_ptr<Layer::Props>& oldProps) {
  super::updateState(changeFlags, oldProps);

  if (changeFlags.propsChanged || changeFlags.viewportChanged) {
    auto props = std::dynamic_pointer_cast<ScatterplotLayer::Props>(this->props());
    float widthMultiplier = props->lineWidthUnits == "pixels" ? this->context->viewport->metersPerPixel() : 1.0;

    ScatterplotLayerUniforms uniforms;
    uniforms.opacity = props->opacity;
    uniforms.radiusScale = props->radiusScale;
    uniforms.radiusMinPixels = props->radiusMinPixels;
    uniforms.radiusMaxPixels = props->radiusMaxPixels;
    uniforms.lineWidthScale = props->lineWidthScale * widthMultiplier;
    uniforms.lineWidthMinPixels = props->lineWidthMinPixels;
    uniforms.lineWidthMaxPixels = props->lineWidthMaxPixels;
    uniforms.stroked = props->stroked ? 1.0f : 0.0f;
    uniforms.filled = props->filled;

    this->_layerUniforms.SetSubData(0, sizeof(ScatterplotLayerUniforms), &uniforms);
  }

  /*
  if (changeFlags.extensionsChanged) {
    this->getAttributeManager().invalidateAll();
  }
  */
}

void ScatterplotLayer::finalizeState() {}

void ScatterplotLayer::drawState(wgpu::RenderPassEncoder pass) {
  for (auto const& model : this->models()) {
    // Layer uniforms are currently bound to index 1
    model->setUniformBuffer(1, this->_layerUniforms);
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
      UniformDescriptor{}, UniformDescriptor{wgpu::ShaderStage::Vertex | wgpu::ShaderStage::Fragment}};
  auto modelOptions = Model::Options{
      vs, fs, attributeSchema, instancedAttributeSchema, uniforms, wgpu::PrimitiveTopology::TriangleStrip};
  auto model = std::make_shared<lumagl::Model>(device, modelOptions);

  // a square that minimally cover the unit circle
  // const positions = [ -1, -1, 0, -1, 1, 0, 1, 1, 0, 1, -1, 0 ];

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

  auto instancedAttributes = this->_attributeManager->update(this->props()->data);
  model->setInstancedAttributes(instancedAttributes);

  return model;
}
