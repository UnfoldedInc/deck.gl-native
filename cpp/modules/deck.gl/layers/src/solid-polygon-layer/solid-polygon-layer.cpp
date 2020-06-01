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
#include "deck.gl/core/src/lib/earcut.hpp"

using namespace deckgl;
using namespace lumagl;

const std::vector<const Property*> propTypeDefs = {
    new PropertyT<bool>{
        "filled", [](const JSONObject* props) { return dynamic_cast<const SolidPolygonLayer::Props*>(props)->filled; },
        [](JSONObject* props, bool value) { return dynamic_cast<SolidPolygonLayer::Props*>(props)->filled = value; },
        true},
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
  // TODO(ilija@unfolded.ai): Guaranteed to crash when this layer goes out of scope, revisit
  auto vertexPositions = std::make_shared<arrow::Field>("vertexPositions", arrow::fixed_size_list(arrow::float32(), 2));
  auto getVertexPositions = [this](const std::shared_ptr<arrow::Table>& table) {
    return this->getVertexPositionData(table);
  };
  this->attributeManager->add(garrow::ColumnBuilder{vertexPositions, getVertexPositions});

  auto vertexValid = std::make_shared<arrow::Field>("vertexValid", arrow::float32());
  auto getVertexValid = [this](const std::shared_ptr<arrow::Table>& table) { return this->getVertexValidData(table); };
  this->attributeManager->add(garrow::ColumnBuilder{vertexValid, getVertexValid});

  // TODO(ilija@unfolded.ai): Revisit type once double precision is in place
  auto positions = std::make_shared<arrow::Field>("positions", arrow::fixed_size_list(arrow::float32(), 3));
  auto getPosition = [this](const std::shared_ptr<arrow::Table>& table) { return this->getPositionData(table); };
  this->attributeManager->add(garrow::ColumnBuilder{positions, getPosition});

  auto elevation = std::make_shared<arrow::Field>("elevations", arrow::float32());
  auto getElevation = [this](const std::shared_ptr<arrow::Table>& table) { return this->getElevationData(table); };
  this->attributeManager->add(garrow::ColumnBuilder{elevation, getElevation});

  auto fillColor = std::make_shared<arrow::Field>("fillColors", arrow::fixed_size_list(arrow::float32(), 4));
  auto getFillColor = [this](const std::shared_ptr<arrow::Table>& table) { return this->getFillColorData(table); };
  this->attributeManager->add(garrow::ColumnBuilder{fillColor, getFillColor});

  auto lineColor = std::make_shared<arrow::Field>("lineColors", arrow::fixed_size_list(arrow::float32(), 4));
  auto getLineColor = [this](const std::shared_ptr<arrow::Table>& table) { return this->getLineColorData(table); };
  this->attributeManager->add(garrow::ColumnBuilder{lineColor, getLineColor});

  this->_attributeSchema = std::make_shared<arrow::Schema>(std::vector{positions, elevation, fillColor, lineColor});

  this->models = {this->_getModels(this->context->device)};
  this->_layerUniforms =
      utils::createBuffer(this->context->device, sizeof(SolidPolygonLayerUniforms), wgpu::BufferUsage::Uniform);
}

void SolidPolygonLayer::updateState(const Layer::ChangeFlags& changeFlags,
                                    const std::shared_ptr<Layer::Props>& oldProps) {
  super::updateState(changeFlags, oldProps);
  this->updateGeometry(changeFlags, oldProps);

  auto props = std::dynamic_pointer_cast<SolidPolygonLayer::Props>(this->props());
  auto oldPropsSPL = std::dynamic_pointer_cast<SolidPolygonLayer::Props>(oldProps);

  // TODO(ilija@unfolded.ai): oldProps isn't being passed right now
  bool regenerateModels = changeFlags.dataChanged.has_value();
  //  bool regenerateModels = oldPropsSPL == nullptr || changeFlags.extensionsChanged ||
  //                          (props->filled != oldPropsSPL->filled) || (props->extruded != oldPropsSPL->extruded);

  if (regenerateModels) {
    this->models = {this->_getModels(this->context->device)};
  }

  if (changeFlags.propsChanged) {
    SolidPolygonLayerUniforms uniforms;
    uniforms.elevationScale = props->elevationScale;
    uniforms.extruded = props->extruded;
    uniforms.wireframe = props->wireframe;
    uniforms.opacity = props->opacity;
    this->_layerUniforms.SetSubData(0, sizeof(SolidPolygonLayerUniforms), &uniforms);
  }
}

void SolidPolygonLayer::updateGeometry(const Layer::ChangeFlags& changeFlags,
                                       const std::shared_ptr<Layer::Props>& oldProps) {
  /*
  const geometryConfigChanged =
    changeFlags.dataChanged ||
    (changeFlags.updateTriggersChanged &&
      (changeFlags.updateTriggersChanged.all || changeFlags.updateTriggersChanged.getPolygon));

    if (!changeFlags.dataChanged) {
      // Base `layer.updateState` only invalidates all attributes on data change
      // Cover the rest of the scenarios here
      this.getAttributeManager().invalidateAll();
    }
  }
  */

  if (changeFlags.dataChanged) {
    this->_processedData = this->_processData(this->props()->data);
  }
}

void SolidPolygonLayer::finalizeState() {}

void SolidPolygonLayer::drawState(wgpu::RenderPassEncoder pass) {
  for (auto const& model : this->getModels()) {
    // Layer uniforms are currently bound to index 1
    model->setUniformBuffer(1, this->_layerUniforms);
    model->draw(pass);
  }
}

// TODO(ilija@unfolded.ai): Remove once specifying constant attributes is possible
auto SolidPolygonLayer::getVertexPositionData(const std::shared_ptr<arrow::Table>& table)
    -> std::shared_ptr<arrow::Array> {
  auto props = std::dynamic_pointer_cast<SolidPolygonLayer::Props>(this->props());
  if (!props) {
    throw std::logic_error("Invalid layer properties");
  }

  return ArrowMapper::mapVector2FloatColumn(table, [](const Row& row) { return mathgl::Vector2<float>{0, 1}; });
}

// TODO(ilija@unfolded.ai): Remove once specifying constant attributes is possible
auto SolidPolygonLayer::getVertexValidData(const std::shared_ptr<arrow::Table>& table)
    -> std::shared_ptr<arrow::Array> {
  auto props = std::dynamic_pointer_cast<SolidPolygonLayer::Props>(this->props());
  if (!props) {
    throw std::logic_error("Invalid layer properties");
  }

  return ArrowMapper::mapFloatColumn(table, [](const Row& row) { return 1.0f; });
}

auto SolidPolygonLayer::getPositionData(const std::shared_ptr<arrow::Table>& table) -> std::shared_ptr<arrow::Array> {
  auto props = std::dynamic_pointer_cast<SolidPolygonLayer::Props>(this->props());
  if (!props) {
    throw std::logic_error("Invalid layer properties");
  }

  // Since table data is already processed using the provided accessors, we just return the processed column data
  return ArrowMapper::mapVector3FloatColumn(table, [](const Row& row) { return row.getVector3<float>("positions"); });
}

auto SolidPolygonLayer::getElevationData(const std::shared_ptr<arrow::Table>& table) -> std::shared_ptr<arrow::Array> {
  auto props = std::dynamic_pointer_cast<SolidPolygonLayer::Props>(this->props());
  if (!props) {
    throw std::logic_error("Invalid layer properties");
  }

  // Since table data is already processed using the provided accessors, we just return the processed column data
  return ArrowMapper::mapFloatColumn(table, [](const Row& row) { return row.getFloat("elevations"); });
}

auto SolidPolygonLayer::getFillColorData(const std::shared_ptr<arrow::Table>& table) -> std::shared_ptr<arrow::Array> {
  auto props = std::dynamic_pointer_cast<SolidPolygonLayer::Props>(this->props());
  if (!props) {
    throw std::logic_error("Invalid layer properties");
  }

  // Since table data is already processed using the provided accessors, we just return the processed column data
  return ArrowMapper::mapVector4FloatColumn(table, [](const Row& row) { return row.getVector4<float>("fillColors"); });
}

auto SolidPolygonLayer::getLineColorData(const std::shared_ptr<arrow::Table>& table) -> std::shared_ptr<arrow::Array> {
  auto props = std::dynamic_pointer_cast<SolidPolygonLayer::Props>(this->props());
  if (!props) {
    throw std::logic_error("Invalid layer properties");
  }

  // Since table data is already processed using the provided accessors, we just return the processed column data
  return ArrowMapper::mapVector4FloatColumn(table, [](const Row& row) { return row.getVector4<float>("lineColors"); });
}

auto SolidPolygonLayer::_getModels(wgpu::Device device) -> std::list<std::shared_ptr<lumagl::Model>> {
  auto filled = this->props()->filled;
  auto extruded = this->props()->extruded;

  std::list<std::shared_ptr<lumagl::Model>> modelsList;

  // If polygon is filled, declare a model that draws the top side of the polygon
  if (filled) {
    // TODO(ilija@unfolded.ai): **arrow**::Fields are already being specified in initializeState, consolidate?
    std::vector<std::shared_ptr<garrow::Field>> attributeFields{
        std::make_shared<garrow::Field>("vertexPositions", wgpu::VertexFormat::Float2),
        std::make_shared<garrow::Field>("vertexValid", wgpu::VertexFormat::Float),
        std::make_shared<garrow::Field>("positions", wgpu::VertexFormat::Float3),
        std::make_shared<garrow::Field>("elevations", wgpu::VertexFormat::Float),
        std::make_shared<garrow::Field>("fillColors", wgpu::VertexFormat::Float4),
        std::make_shared<garrow::Field>("lineColors", wgpu::VertexFormat::Float4)};
    auto attributeSchema = std::make_shared<lumagl::garrow::Schema>(attributeFields);
    auto instancedAttributeSchema = std::make_shared<garrow::Schema>(std::vector<std::shared_ptr<garrow::Field>>{});
    std::vector<UniformDescriptor> uniforms = {
        UniformDescriptor{}, UniformDescriptor{wgpu::ShaderStage::Vertex | wgpu::ShaderStage::Fragment}};

    auto modelOptions = Model::Options{
        vst, fs, attributeSchema, instancedAttributeSchema, uniforms, wgpu::PrimitiveTopology::TriangleList};

    auto model = std::make_shared<lumagl::Model>(device, modelOptions);
    // Make sure we've processed raw data before setting attributes
    if (this->_processedData) {
      model->setAttributes(this->attributeManager->update(this->_processedData));
    }

    modelsList.push_back(model);
  }

  // If polygon is extruded, declare a model that draws polygon sides
  if (extruded) {
    // TODO(ilija@unfolded.ai): **arrow**::Fields are already being specified in initializeState, consolidate?
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
    // top right - top left - bootom left - bottom right
    std::vector<mathgl::Vector2<float>> positionData = {{1, 1}, {0, 1}, {0, 0}, {1, 0}};
    std::vector<std::shared_ptr<garrow::Array>> attributeArrays{
        std::make_shared<garrow::Array>(this->context->device, positionData, wgpu::BufferUsage::Vertex)};
    model->setAttributes(std::make_shared<garrow::Table>(attributeSchema, attributeArrays));

    auto instancedAttributes = this->attributeManager->update(this->props()->data);
    model->setInstancedAttributes(instancedAttributes);

    modelsList.push_back(model);
  }

  return modelsList;
}

auto SolidPolygonLayer::_processData(const std::shared_ptr<arrow::Table>& data) -> std::shared_ptr<arrow::Table> {
  // We build the geometry based on input polygon data
  // Polygon data has to be tessallated, and the data table has to be rebuilt so that it contains tessellated points
  // that can be used to draw polygon triangles
  arrow::MemoryPool* pool = arrow::default_memory_pool();

  arrow::FixedSizeListBuilder positionListBuilder{pool, std::make_shared<arrow::FloatBuilder>(pool), 3};
  arrow::FloatBuilder& positionBuilder = *(static_cast<arrow::FloatBuilder*>(positionListBuilder.value_builder()));

  arrow::FloatBuilder elevationBuilder{pool};

  arrow::FixedSizeListBuilder fillColorListBuilder{pool, std::make_shared<arrow::FloatBuilder>(pool), 4};
  arrow::FloatBuilder& fillColorBuilder = *(static_cast<arrow::FloatBuilder*>(fillColorListBuilder.value_builder()));

  arrow::FixedSizeListBuilder lineColorListBuilder{pool, std::make_shared<arrow::FloatBuilder>(pool), 4};
  arrow::FloatBuilder& lineColorBuilder = *(static_cast<arrow::FloatBuilder*>(lineColorListBuilder.value_builder()));

  // Approximate the amount of space we'll need in these buffers, as we don't know the total polygon count without
  // inspecting the entire table. Assuming 4 pointers per polygon
  auto approximateElementCount = data->num_rows() * 4;
  if (!positionListBuilder.Reserve(approximateElementCount).ok() ||
      !elevationBuilder.Reserve(approximateElementCount).ok() ||
      !fillColorListBuilder.Reserve(approximateElementCount).ok() ||
      !lineColorListBuilder.Reserve(approximateElementCount).ok() ||
      !positionBuilder.Reserve(approximateElementCount * 3).ok() ||
      !fillColorBuilder.Reserve(approximateElementCount * 4).ok() ||
      !lineColorBuilder.Reserve(approximateElementCount * 4).ok()) {
    probegl::WarningLog() << "Unable to reserve sufficient space for processing polygon data";
    return nullptr;
  }

  using Point = std::array<float, 3>;

  // Iterate over the original data set
  for (auto i = 0; i < data->num_rows(); i++) {
    // Extract row data for this polygon
    auto row = Row{data, i};
    auto polygon = this->props()->getPolygon(row);
    auto elevation = this->props()->getElevation(row);
    auto fillColor = this->props()->getFillColor(row);
    auto lineColor = this->props()->getLineColor(row);

    // Convert polygon points to a format needed by the tessellator
    // TODO(ilija@unfolded.ai): Avoid copying the data by either conforming Vector3 (or its subclass) so that it can be
    // passed to earcut, or having getPolygon accessor return a vector of arrays
    std::vector<Point> points;
    points.reserve(polygon.size());
    for (const auto& point : polygon) {
      points.push_back({point.x, point.y, point.z});
    }

    // Tessellate the polygon
    auto indices = mapbox::earcut(std::vector<std::vector<Point>>{points});
    // Create a new row in the resulting table for each point of the tessellated data set
    // We copy over the data for each point from the polygon row in the original data set is belongs to
    for (const auto& index : indices) {
      auto point = polygon[index];

      if (!positionListBuilder.Append().ok() || !fillColorListBuilder.Append().ok() ||
          !lineColorListBuilder.Append().ok()) {
        probegl::WarningLog() << "Unable to append polygon list data";
        return nullptr;
      }
      if (!positionBuilder.AppendValues(&point.x, 3).ok() || !elevationBuilder.Append(elevation).ok() ||
          !fillColorBuilder.AppendValues(&fillColor.x, 4).ok() ||
          !lineColorBuilder.AppendValues(&lineColor.x, 4).ok()) {
        probegl::WarningLog() << "Unable to append polygon data";
        return nullptr;
      }
    }
  }

  std::shared_ptr<arrow::Array> positionArray;
  std::shared_ptr<arrow::Array> elevationArray;
  std::shared_ptr<arrow::Array> fillColorArray;
  std::shared_ptr<arrow::Array> lineColorArray;
  if (!positionListBuilder.Finish(&positionArray).ok() || !elevationBuilder.Finish(&elevationArray).ok() ||
      !fillColorListBuilder.Finish(&fillColorArray).ok() || !lineColorListBuilder.Finish(&lineColorArray).ok()) {
    probegl::WarningLog() << "Unable to extract processed polygon data";
    return nullptr;
  }

  return arrow::Table::Make(this->_attributeSchema, {positionArray, elevationArray, fillColorArray, lineColorArray});
}
