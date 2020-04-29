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

#ifndef DECKGL_LAYERS_SOLIDPOLYGON_LAYER_H
#define DECKGL_LAYERS_SOLIDPOLYGON_LAYER_H

#include <memory>
#include <string>

#include "deck.gl/core.h"

namespace deckgl {

class SolidPolygonLayer : public Layer{
 public:
  class Props;
  explicit SolidPolygonLayer(std::shared_ptr<SolidPolygonLayer::Props> props)
      : Layer{std::dynamic_pointer_cast<Layer::Props>(props)} {}
  auto props() { return std::dynamic_pointer_cast<Layer::Props>(this->_props); }

  auto getPolygon(const std::shared_ptr<arrow::Table>& table) -> std::shared_ptr<arrow::Array>;
  auto getElevationData(const std::shared_ptr<arrow::Table>& table) -> std::shared_ptr<arrow::Array>;
  auto getFillColorData(const std::shared_ptr<arrow::Table>& table) -> std::shared_ptr<arrow::Array>;
  auto getLineColorData(const std::shared_ptr<arrow::Table>& table) -> std::shared_ptr<arrow::Array>;
 protected:
  void initializeState() override;
  // auto getPickingInfo() override;
  void updateState(const ChangeFlags&, const Layer::Props* oldProps) override;
  // void updateGeometry(const ChangeFlag &, const Layer::Props* oldProps) override;
  void finalizeState() override;
  void drawState(wgpu::RenderPassEncoder pass) override;
 private:
  auto _getModel(wgpu::Device device) -> std::shared_ptr<lumagl::Model>;
};

class SolidPolygonLayer::Props : public Layer::Props {
 public:
  using super = Layer::Props;
  static constexpr const char* getTypeName() { return "SolidPolygonLayer"; }

  // Property Type Machinery
  auto getProperties() const -> const Properties* override;
  auto makeComponent(std::shared_ptr<Component::Props> props) const -> SolidPolygonLayer* override {
    return new SolidPolygonLayer{std::dynamic_pointer_cast<SolidPolygonLayer::Props>(props)};
  }

  bool filled{true};
  bool extruded{false};
  bool wireframe{false};
  bool material{true};

  // _normalize in JS, intended to be private?
  bool normalize{true};

  float elevationScale{1.0};

  std::function<ArrowMapper::Vector4FloatAccessor> getPolygon{
    [](const Row& row) { return row.getFloatVector4("polygon"); }};
  std::function<ArrowMapper::FloatAccessor> getElevation{
    [](const Row& row) { return 1000.0; }};
  std::function<ArrowMapper::Vector4FloatAccessor> getFillColor{
    [](const Row&) { return mathgl::Vector4<float>(0.0, 0.0, 0.0, 255.0); }};
  std::function<ArrowMapper::Vector4FloatAccessor> getLineColor{
    [](const Row&) { return mathgl::Vector4<float>(0.0, 0.0, 0.0, 255.0); }};
};

}  // namespace deckgl
#endif  // DECKGL_LAYERS_SOLIDPOLYGON_LAYER_H
