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

/// \brief Layer subclass that interprets scatterplot data.
class ScatterplotLayer : public Layer {
 public:
  using super = Layer;

  class Props;
  explicit ScatterplotLayer(std::shared_ptr<ScatterplotLayer::Props> props)
      : Layer{std::dynamic_pointer_cast<Layer::Props>(props)} {}
  auto props() { return std::dynamic_pointer_cast<Layer::Props>(this->_props); }

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

/// \brief A set of properties that describe a ScatterplotLayer.
class ScatterplotLayer::Props : public Layer::Props {
 public:
  using super = Layer::Props;

  /// \brief Determines whether the point should be filled or not.
  bool filled{true};

  /// \brief Determines whether the point should have an outline or not.
  bool stroked{false};

  // TODO(ilija@unfolded.ai): Should be an enum
  /// \brief Units in which the width of the outline is specified.
  std::string lineWidthUnits{"meters"};

  /// \brief Scale to use for point outline data.
  float lineWidthScale{1.0};

  /// \brief Minimum width of the outline, in pixels.
  float lineWidthMinPixels{0.0};

  /// \brief Maximum width of the line, in pixels.
  float lineWidthMaxPixels{std::numeric_limits<float>::max()};

  /// \brief Scale to use for point radius data.
  float radiusScale{1.0};

  /// \brief Minimum radius of the point, in pixels.
  float radiusMinPixels{0.0};

  /// \brief Maximum radius of the point, in pixels.
  float radiusMaxPixels{std::numeric_limits<float>::max()};

  /// Property accessors
  std::function<ArrowMapper::Vector3FloatAccessor> getPosition{
      [](const Row& row) { return row.getVector3<float>("position"); }};
  std::function<ArrowMapper::FloatAccessor> getRadius{[](const Row&) { return 1.0; }};
  std::function<ArrowMapper::Vector4FloatAccessor> getFillColor{
      [](const Row&) { return mathgl::Vector4<float>(0.0, 0.0, 0.0, 255.0); }};
  std::function<ArrowMapper::Vector4FloatAccessor> getLineColor{
      [](const Row&) { return mathgl::Vector4<float>(0.0, 0.0, 0.0, 255.0); }};
  std::function<ArrowMapper::FloatAccessor> getLineWidth{[](auto row) { return 1.0; }};

  // Property Type Machinery
  static constexpr const char* getTypeName() { return "ScatterplotLayer"; }
  auto getProperties() const -> const std::shared_ptr<Properties> override;
  auto makeComponent(std::shared_ptr<Component::Props> props) const -> std::shared_ptr<Component> override {
    return std::make_shared<ScatterplotLayer>(std::dynamic_pointer_cast<ScatterplotLayer::Props>(props));
  }
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
