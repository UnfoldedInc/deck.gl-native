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

#include "./solid-polygon-layer.h"  // NOLINT(build/include)

#include "./solid-polygon-layer-fragment.glsl.h"
#include "./solid-polygon-layer-vertex-main.glsl.h"
#include "./solid-polygon-layer-vertex-side.glsl.h"
#include "./solid-polygon-layer-vertex-top.glsl.h"
#include "deck.gl/core.h"

using namespace deckgl;
using namespace lumagl;

const std::vector<const Property*> propTypeDefs = {
    new PropertyT<bool>{
        "filled", [](const JSONObject* props) { return dynamic_cast<const SolidPolygonLayer::Props*>(props)->filled; },
        [](JSONObject* props, bool value) { return dynamic_cast<SolidPolygonLayer::Props*>(props)->filled = value; },
        true},
    new PropertyT<bool>{
        "extruded",
        [](const JSONObject* props) { return dynamic_cast<const SolidPolygonLayer::Props*>(props)->extruded; },
        [](JSONObject* props, bool value) { return dynamic_cast<SolidPolygonLayer::Props*>(props)->extruded = value; },
        false},
    new PropertyT<bool>{
        "wireframe",
        [](const JSONObject* props) { return dynamic_cast<const SolidPolygonLayer::Props*>(props)->wireframe; },
        [](JSONObject* props, bool value) { return dynamic_cast<SolidPolygonLayer::Props*>(props)->wireframe = value; },
        false},
    new PropertyT<bool>{
        "material",
        [](const JSONObject* props) { return dynamic_cast<const SolidPolygonLayer::Props*>(props)->material; },
        [](JSONObject* props, bool value) { return dynamic_cast<SolidPolygonLayer::Props*>(props)->material = value; },
        true},
    new PropertyT<bool>{
        "normalize",
        [](const JSONObject* props) { return dynamic_cast<const SolidPolygonLayer::Props*>(props)->normalize; },
        [](JSONObject* props, bool value) { return dynamic_cast<SolidPolygonLayer::Props*>(props)->normalize = value; },
        true},
    new PropertyT<bool>{
        "stroked",
        [](const JSONObject* props) { return dynamic_cast<const SolidPolygonLayer::Props*>(props)->stroked; },
        [](JSONObject* props, bool value) { return dynamic_cast<SolidPolygonLayer::Props*>(props)->stroked = value; },
        false},
    new PropertyT<float>{
        "elevationScale",
        [](const JSONObject* props) { return dynamic_cast<const SolidPolygonLayer::Props*>(props)->elevationScale; },
        [](JSONObject* props, bool value) {
          return dynamic_cast<SolidPolygonLayer::Props*>(props)->elevationScale = value;
        },
        1.0}};

auto SolidPolygonLayer::Props::getProperties() const -> const Properties* {
  static Properties properties{Properties::from<SolidPolygonLayer::Props>("SolidPolygonLayer", propTypeDefs)};
  return &properties;
}

void SolidPolygonLayer::initializeState() {
  // Using lambdas over std::bind - potential C++ retain cycle issue

  auto polygon =
      std::make_shared<arrow::Field>("instancePolygons", arrow::list(arrow::fixed_size_list(arrow::float32(), 3)));
  auto getPolygon = [this](const std::shared_ptr<arrow::Table>& table) { return this->getPolygonData(table); };
  this->attributeManager->add(garrow::ColumnBuilder{polygon, getPolygon});

  auto elevation = std::make_shared<arrow::Field>("instanceElevations", arrow::float32());
  auto getElevation = [this](const std::shared_ptr<arrow::Table>& table) { return this->getElevationData(table); };
  this->attributeManager->add(garrow::ColumnBuilder{elevation, getElevation});

  auto fillColor = std::make_shared<arrow::Field>("instanceFillColors", arrow::fixed_size_list(arrow::float32(), 4));
  auto getFillColor = [this](const std::shared_ptr<arrow::Table>& table) { return this->getFillColorData(table); };
  this->attributeManager->add(garrow::ColumnBuilder{fillColor, getFillColor});

  auto lineColor = std::make_shared<arrow::Field>("instanceLineColors", arrow::fixed_size_list(arrow::float32(), 4));
  auto getLineColor = [this](const std::shared_ptr<arrow::Table>& table) { return this->getLineColorData(table); };
  this->attributeManager->add(garrow::ColumnBuilder{lineColor, getLineColor});

  this->models = {this->_getModels(this->context->device)};
  this->_layerUniforms =
      utils::createBuffer(this->context->device, sizeof(SolidPolygonLayerUniforms), wgpu::BufferUsage::Uniform);
}

/*
auto SolidPolygonLayer::getPickingInfo(params) {

    const info = super.getPickingInfo(params);
    const {object, index} = info;

    if (object && object.__source) {
      // data is wrapped
      info.object = this.props.data.find(d => d.__source.index === index);
    }
    return info;

}
*/

void SolidPolygonLayer::updateStateSPL(const Layer::ChangeFlags& changeFlags,
                                       const SolidPolygonLayer::Props* oldProps) {
  updateGeometry(changeFlags, oldProps);

  auto props = std::dynamic_pointer_cast<SolidPolygonLayer::Props>(this->props());

  bool regenerateModels =
      (changeFlags.extensionsChanged || (props->filled != oldProps->filled) || (props->extruded != oldProps->extruded));

  SolidPolygonLayerUniforms uniforms;
  uniforms.elevationScale = props->elevationScale;
  uniforms.extruded = props->extruded;
  uniforms.wireframe = props->wireframe;
  uniforms.opacity = props->opacity;

  this->_layerUniforms.SetSubData(0, sizeof(SolidPolygonLayerUniforms), &uniforms);

  if (regenerateModels) {
    this->models.clear();
  }
  // set state
  // invalidate all in attr manager

  /*
    super.updateState(updateParams);

    this.updateGeometry(updateParams);

    const {props, oldProps, changeFlags} = updateParams;
    const attributeManager = this.getAttributeManager();

    const regenerateModels =
      changeFlags.extensionsChanged ||
      props.filled !== oldProps.filled ||
      props.extruded !== oldProps.extruded;

    if (regenerateModels) {
      if (this.state.models) {
        this.state.models.forEach(model => model.delete());
      }

      this.setState(this._getModels(this.context.gl));
      attributeManager.invalidateAll();
    }
  */
}

void SolidPolygonLayer::updateGeometry(const Layer::ChangeFlags& changeFlags, const Layer::Props* oldProps) {
  /*
    const geometryConfigChanged =
      changeFlags.dataChanged ||
      (changeFlags.updateTriggersChanged &&
        (changeFlags.updateTriggersChanged.all || changeFlags.updateTriggersChanged.getPolygon));

    // When the geometry config  or the data is changed,
    // tessellator needs to be invoked
    if (geometryConfigChanged) {
      const {polygonTesselator} = this.state;
      const buffers = props.data.attributes || {};
      polygonTesselator.updateGeometry({
        data: props.data,
        normalize: props._normalize,
        geometryBuffer: buffers.getPolygon,
        buffers,
        getGeometry: props.getPolygon,
        positionFormat: props.positionFormat,
        fp64: this.use64bitPositions(),
        dataChanged: changeFlags.dataChanged
      });

      this.setState({
        numInstances: polygonTesselator.instanceCount,
        startIndices: polygonTesselator.vertexStarts
      });

      if (!changeFlags.dataChanged) {
        // Base `layer.updateState` only invalidates all attributes on data change
        // Cover the rest of the scenarios here
        this.getAttributeManager().invalidateAll();
      }
    }
    */
}

void SolidPolygonLayer::finalizeState() {}

void SolidPolygonLayer::drawState(wgpu::RenderPassEncoder pass) {
  this->updateState(Layer::ChangeFlags{}, nullptr);

  for (auto const& model : this->getModels()) {
    // Layer uniforms are currently bound to index 1
    model->setUniformBuffer(1, this->_layerUniforms);
    model->draw(pass);
  }
}

auto SolidPolygonLayer::getPolygonData(const std::shared_ptr<arrow::Table>& table) -> std::shared_ptr<arrow::Array> {
  auto props = std::dynamic_pointer_cast<SolidPolygonLayer::Props>(this->props());
  if (!props) {
    throw std::logic_error("Invalid layer properties");
  }

  return ArrowMapper::mapVector3FloatColumn(table, props->getPolygon);
}

auto SolidPolygonLayer::getElevationData(const std::shared_ptr<arrow::Table>& table) -> std::shared_ptr<arrow::Array> {
  auto props = std::dynamic_pointer_cast<SolidPolygonLayer::Props>(this->props());
  if (!props) {
    throw std::logic_error("Invalid layer properties");
  }

  return ArrowMapper::mapFloatColumn(table, props->getElevation);
}

auto SolidPolygonLayer::getFillColorData(const std::shared_ptr<arrow::Table>& table) -> std::shared_ptr<arrow::Array> {
  auto props = std::dynamic_pointer_cast<SolidPolygonLayer::Props>(this->props());
  if (!props) {
    throw std::logic_error("Invalid layer properties");
  }

  return ArrowMapper::mapVector4FloatColumn(table, props->getFillColor);
}

auto SolidPolygonLayer::getLineColorData(const std::shared_ptr<arrow::Table>& table) -> std::shared_ptr<arrow::Array> {
  auto props = std::dynamic_pointer_cast<SolidPolygonLayer::Props>(this->props());
  if (!props) {
    throw std::logic_error("Invalid layer properties");
  }

  return ArrowMapper::mapVector4FloatColumn(table, props->getLineColor);
}

auto SolidPolygonLayer::_getModels(wgpu::Device device) -> std::list<std::shared_ptr<lumagl::Model>> {
  // JS returns both models for the top and side if properties are set as such, so return a list
  // of models with id = either 'id-top' or 'id-side'
  auto id = this->props()->id;
  auto filled = this->props()->filled;
  auto extruded = this->props()->extruded;

  std::list<std::shared_ptr<lumagl::Model>> modelsList;

  if (filled) {
    // make new model using vsTop
    // TODO(randy@unfolded.ai): Remove instanced fields, add (wireframe, isSideVertex) uniforms to top
    std::vector<std::shared_ptr<garrow::Field>> attributeFields{
        std::make_shared<garrow::Field>("vertexPositions", wgpu::VertexFormat::Float2),
        std::make_shared<garrow::Field>("vertexValid", wgpu::VertexFormat::Float),
        std::make_shared<garrow::Field>("positions", wgpu::VertexFormat::Float3),
        std::make_shared<garrow::Field>("elevations", wgpu::VertexFormat::Float),
        std::make_shared<garrow::Field>("fillColors", wgpu::VertexFormat::Float4),
        std::make_shared<garrow::Field>("lineColors", wgpu::VertexFormat::Float4)};

    auto attributeSchema = std::make_shared<lumagl::garrow::Schema>(attributeFields);

    // TODO(randy@unfolded.ai): Although instanced fields don't seem to be used in the JS vertex-top shader,
    //                          leaving as nullptr causes a segmentation fault. Using placeholder for now.
    std::vector<std::shared_ptr<garrow::Field>> instancedFields{
        std::make_shared<garrow::Field>("placeholder", wgpu::VertexFormat::Float)};
    auto instancedAttributeSchema = std::make_shared<lumagl::garrow::Schema>(instancedFields);

    std::vector<UniformDescriptor> uniforms = {
        UniformDescriptor{}, UniformDescriptor{wgpu::ShaderStage::Vertex | wgpu::ShaderStage::Fragment}};

    auto modelOptions = Model::Options{
        vss, fs, attributeSchema, instancedAttributeSchema, uniforms, wgpu::PrimitiveTopology::TriangleList};

    auto model = std::make_shared<lumagl::Model>(device, modelOptions);
    std::vector<mathgl::Vector2<float>> positionData = {{0, 1}};
    std::vector<std::shared_ptr<garrow::Array>> attributeArrays{
        std::make_shared<garrow::Array>(this->context->device, positionData, wgpu::BufferUsage::Vertex)};
    model->setAttributes(std::make_shared<garrow::Table>(attributeSchema, attributeArrays));

    modelsList.push_back(model);
  }
  if (extruded) {
    // make new model using vsSide
    std::vector<std::shared_ptr<garrow::Field>> attributeFields{
        std::make_shared<garrow::Field>("vertexPositions", wgpu::VertexFormat::Float2),
        std::make_shared<garrow::Field>("vertexValid", wgpu::VertexFormat::Float)};

    auto attributeSchema = std::make_shared<lumagl::garrow::Schema>(attributeFields);

    std::vector<std::shared_ptr<garrow::Field>> instancedFields{
        std::make_shared<garrow::Field>("instancePositions", wgpu::VertexFormat::Float3),
        std::make_shared<garrow::Field>("nextPositions", wgpu::VertexFormat::Float3),
        std::make_shared<garrow::Field>("instanceElevations", wgpu::VertexFormat::Float),
        std::make_shared<garrow::Field>("instanceFillColors", wgpu::VertexFormat::Float4),
        std::make_shared<garrow::Field>("instanceLineColors", wgpu::VertexFormat::Float4)};
    auto instancedAttributeSchema = std::make_shared<lumagl::garrow::Schema>(instancedFields);

    std::vector<UniformDescriptor> uniforms = {
        UniformDescriptor{}, UniformDescriptor{wgpu::ShaderStage::Vertex | wgpu::ShaderStage::Fragment}};

    auto modelOptions =
        Model::Options{vss, fs, attributeSchema, instancedAttributeSchema, uniforms, wgpu::PrimitiveTopology::LineList};

    auto model = std::make_shared<lumagl::Model>(device, modelOptions);
    std::vector<mathgl::Vector2<float>> positionData = {{-1, 1}, {-1, -1}, {1, 1}, {1, -1}};
    std::vector<std::shared_ptr<garrow::Array>> attributeArrays{
        std::make_shared<garrow::Array>(this->context->device, positionData, wgpu::BufferUsage::Vertex)};
    model->setAttributes(std::make_shared<garrow::Table>(attributeSchema, attributeArrays));

    auto instancedAttributes = this->attributeManager->update(this->props()->data);
    model->setInstancedAttributes(instancedAttributes);

    modelsList.push_back(model);
  }
  return modelsList;
}
