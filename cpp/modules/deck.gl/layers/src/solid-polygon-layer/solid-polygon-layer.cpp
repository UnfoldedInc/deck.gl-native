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
  auto polygon = std::make_shared<arrow::Field>("instancePolygons", arrow::fixed_size_list(arrow::float32(), 4));
  auto getPolygon = std::bind(&SolidPolygonLayer::getPolygonData, this, std::placeholders::_1);
  this->attributeManager->add(garrow::ColumnBuilder{polygon, getPolygon});

  auto elevation = std::make_shared<arrow::Field>("instanceElevations", arrow::float32());
  auto getElevation = std::bind(&SolidPolygonLayer::getElevationData, this, std::placeholders::_1);
  this->attributeManager->add(garrow::ColumnBuilder{elevation, getElevation});

  auto fillColor = std::make_shared<arrow::Field>("instanceFillColors", arrow::fixed_size_list(arrow::float32(), 4));
  auto getFillColor = std::bind(&SolidPolygonLayer::getFillColorData, this, std::placeholders::_1);
  this->attributeManager->add(garrow::ColumnBuilder{fillColor, getFillColor});

  auto lineColor = std::make_shared<arrow::Field>("instanceLineColors", arrow::fixed_size_list(arrow::float32(), 4));
  auto getLineColor = std::bind(&SolidPolygonLayer::getLineColorData, this, std::placeholders::_1);
  this->attributeManager->add(garrow::ColumnBuilder{lineColor, getLineColor});
}

void SolidPolygonLayer::updateState(const Layer::ChangeFlags& changeFlags, const Layer::Props* oldProps) {}

void SolidPolygonLayer::finalizeState() {}

void SolidPolygonLayer::drawState(wgpu::RenderPassEncoder pass) {}

auto SolidPolygonLayer::getPolygonData(const std::shared_ptr<arrow::Table>& table) -> std::shared_ptr<arrow::Array> {
  auto props = std::dynamic_pointer_cast<SolidPolygonLayer::Props>(this->props());
  if (!props) {
    throw std::logic_error("Invalid layer properties");
  }

  return ArrowMapper::mapVector4FloatColumn(table, props->getPolygon);
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
