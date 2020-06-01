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

#ifndef DECKGL_LAYERS_SCATTERPLOT_LAYER_H
#define DECKGL_LAYERS_SCATTERPLOT_LAYER_H

#include <limits>
#include <memory>
#include <string>

#include "deck.gl/core.h"
#include "luma.gl/garrow.h"
#include "luma.gl/webgpu.h"

namespace deckgl {

class ScatterplotLayer : public Layer {
 public:
  using super = Layer;

  class Props;
  explicit ScatterplotLayer(std::shared_ptr<ScatterplotLayer::Props> props)
      : Layer{std::dynamic_pointer_cast<Layer::Props>(props)} {}
  auto props() { return std::dynamic_pointer_cast<Layer::Props>(this->_props); }

  // TODO(ilija@unfolded.ai): These should be protected. Figure out how to test them without polluting with friend
  // classes
  auto getPositionData(const std::shared_ptr<arrow::Table>& table) -> std::shared_ptr<arrow::Array>;
  auto getRadiusData(const std::shared_ptr<arrow::Table>& table) -> std::shared_ptr<arrow::Array>;
  auto getFillColorData(const std::shared_ptr<arrow::Table>& table) -> std::shared_ptr<arrow::Array>;
  auto getLineColorData(const std::shared_ptr<arrow::Table>& table) -> std::shared_ptr<arrow::Array>;
  auto getLineWidthData(const std::shared_ptr<arrow::Table>& table) -> std::shared_ptr<arrow::Array>;

 protected:
  void initializeState() override;
  void updateState(const ChangeFlags&, const std::shared_ptr<Layer::Props>& oldProps) override;
  void finalizeState() override;
  void drawState(wgpu::RenderPassEncoder pass) override;

 private:
  auto _getModel(wgpu::Device device) -> std::shared_ptr<lumagl::Model>;

  wgpu::Buffer _layerUniforms;
};

class ScatterplotLayer::Props : public Layer::Props {
 public:
  using super = Layer::Props;
  static constexpr const char* getTypeName() { return "ScatterplotLayer"; }

  // Property Type Machinery
  auto getProperties() const -> const Properties* override;
  auto makeComponent(std::shared_ptr<Component::Props> props) const -> ScatterplotLayer* override {
    return new ScatterplotLayer{std::dynamic_pointer_cast<ScatterplotLayer::Props>(props)};
  }

  bool filled{true};
  bool stroked{false};

  std::string lineWidthUnits{"meters"};
  float lineWidthScale{1.0};
  float lineWidthMinPixels{0.0};
  float lineWidthMaxPixels{std::numeric_limits<float>::max()};

  float radiusScale{1.0};
  float radiusMinPixels{0.0};                                // min point radius in pixels
  float radiusMaxPixels{std::numeric_limits<float>::max()};  // max point radius in pixels

  /// Property accessors
  std::function<ArrowMapper::Vector3FloatAccessor> getPosition{
      [](const Row& row) { return row.getVector3<float>("position"); }};
  std::function<ArrowMapper::FloatAccessor> getRadius{[](const Row&) { return 1.0; }};
  std::function<ArrowMapper::Vector4FloatAccessor> getFillColor{
      [](const Row&) { return mathgl::Vector4<float>(0.0, 0.0, 0.0, 255.0); }};
  std::function<ArrowMapper::Vector4FloatAccessor> getLineColor{
      [](const Row&) { return mathgl::Vector4<float>(0.0, 0.0, 0.0, 255.0); }};
  std::function<ArrowMapper::FloatAccessor> getLineWidth{[](auto row) { return 1.0; }};
};

/// The order of fields in this structure is crucial for it to be mapped to its GLSL counterpart properly.
/// bool has a 4-byte alignment in GLSL.
/// https://learnopengl.com/Advanced-OpenGL/Advanced-GLSL
struct ScatterplotLayerUniforms {
  float opacity;
  float radiusScale;
  float radiusMinPixels;
  float radiusMaxPixels;
  float lineWidthScale;
  float lineWidthMinPixels;
  float lineWidthMaxPixels;
  float stroked;
  alignas(4) bool filled;
};

}  // namespace deckgl

#endif  // DECKGL_LAYERS_SCATTERPLOT_LAYER_H
