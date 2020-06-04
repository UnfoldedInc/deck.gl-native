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

#include <list>
#include <memory>
#include <string>
#include <vector>

#include "deck.gl/core.h"

namespace deckgl {

class SolidPolygonLayer : public Layer {
 public:
  using super = Layer;

  class Props;
  explicit SolidPolygonLayer(std::shared_ptr<SolidPolygonLayer::Props> props)
      : Layer{std::dynamic_pointer_cast<Layer::Props>(props)} {}
  auto props() { return std::dynamic_pointer_cast<SolidPolygonLayer::Props>(this->_props); }

  auto processData(const std::shared_ptr<arrow::Table>& data) -> std::shared_ptr<arrow::Table>;

  auto getVertexPositionData(const std::shared_ptr<arrow::Table>& table) -> std::shared_ptr<arrow::Array>;
  auto getVertexValidData(const std::shared_ptr<arrow::Table>& table) -> std::shared_ptr<arrow::Array>;
  auto getPositionData(const std::shared_ptr<arrow::Table>& table) -> std::shared_ptr<arrow::Array>;
  auto getElevationData(const std::shared_ptr<arrow::Table>& table) -> std::shared_ptr<arrow::Array>;
  auto getFillColorData(const std::shared_ptr<arrow::Table>& table) -> std::shared_ptr<arrow::Array>;
  auto getLineColorData(const std::shared_ptr<arrow::Table>& table) -> std::shared_ptr<arrow::Array>;

 protected:
  void initializeState() override;
  // auto getPickingInfo() override;
  void updateState(const ChangeFlags&, const std::shared_ptr<Layer::Props>& oldProps) override;
  void updateGeometry(const ChangeFlags&, const std::shared_ptr<Layer::Props>& oldProps);
  void finalizeState() override;
  void drawState(wgpu::RenderPassEncoder pass) override;

 private:
  auto _getModels(wgpu::Device device) -> std::list<std::shared_ptr<lumagl::Model>>;

  wgpu::Buffer _layerUniforms;

  std::shared_ptr<arrow::Table> _processedData;
  std::vector<uint32_t> _tesselatedIndices;
};

class SolidPolygonLayer::Props : public Layer::Props {
 public:
  using super = Layer::Props;

  bool filled{true};
  // TODO(ilija@unfolded.ai): Extrusion currently not supported
  const bool extruded{false};
  // TODO(ilija@unfolded.ai): Wireframe currently not supported
  const bool wireframe{false};

  float elevationScale{1.0};

  std::function<ArrowMapper::ListVector3FloatAccessor> getPolygon{
      [](const Row& row) { return row.getVector3List<float>("polygon"); }};
  std::function<ArrowMapper::FloatAccessor> getElevation{[](const Row& row) { return 1000.0; }};
  std::function<ArrowMapper::Vector4FloatAccessor> getFillColor{
      [](const Row&) { return mathgl::Vector4<float>(0.0, 0.0, 0.0, 255.0); }};
  std::function<ArrowMapper::Vector4FloatAccessor> getLineColor{
      [](const Row&) { return mathgl::Vector4<float>(0.0, 0.0, 0.0, 255.0); }};

  // Property Type Machinery
  static constexpr const char* getTypeName() { return "SolidPolygonLayer"; }
  auto getProperties() const -> const std::shared_ptr<Properties> override;
  auto makeComponent(std::shared_ptr<Component::Props> props) const -> std::shared_ptr<Component> override {
    return std::make_shared<SolidPolygonLayer>(std::dynamic_pointer_cast<SolidPolygonLayer::Props>(props));
  }
};

/// The order of fields in this structure is crucial for it to be mapped to its GLSL counterpart properly.
/// Using uint32_t in place of bool in order to avoid packing issues, even when specifying 4-byte alignment.
/// https://learnopengl.com/Advanced-OpenGL/Advanced-GLSL
struct SolidPolygonLayerUniforms {
  uint32_t extruded;
  uint32_t wireframe;
  float elevationScale;
  float opacity;
};

}  // namespace deckgl

#endif  // DECKGL_LAYERS_SOLIDPOLYGON_LAYER_H
