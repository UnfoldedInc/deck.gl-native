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

#ifndef DECKGL_LAYERS_LINE_LAYER_H
#define DECKGL_LAYERS_LINE_LAYER_H

#include <arrow/array.h>
#include <arrow/table.h>

#include <limits>
#include <memory>
#include <string>

#include "deck.gl/core.h"
#include "luma.gl/garrow.h"
#include "luma.gl/webgpu.h"

namespace deckgl {

/// \brief Layer subclass that interprets line data.
class LineLayer : public Layer {
 public:
  using super = Layer;

  class Props;
  explicit LineLayer(std::shared_ptr<LineLayer::Props> props) : Layer{std::dynamic_pointer_cast<Layer::Props>(props)} {}
  auto props() { return std::dynamic_pointer_cast<Layer::Props>(this->_props); }

  auto getSourcePositionData(const std::shared_ptr<arrow::Table>& table) -> std::shared_ptr<arrow::Array>;
  auto getTargetPositionData(const std::shared_ptr<arrow::Table>& table) -> std::shared_ptr<arrow::Array>;
  auto getColorData(const std::shared_ptr<arrow::Table>& table) -> std::shared_ptr<arrow::Array>;
  auto getWidthData(const std::shared_ptr<arrow::Table>& table) -> std::shared_ptr<arrow::Array>;

 protected:
  void initializeState() override;
  void updateState(const Layer::ChangeFlags&, const std::shared_ptr<Layer::Props>& oldProps) override;
  void finalizeState() override;
  void drawState(wgpu::RenderPassEncoder pass) override;

 private:
  auto _getModel(wgpu::Device) -> std::shared_ptr<lumagl::Model>;

  wgpu::Buffer _layerUniforms;
};

/// \brief A set of properties that describe a LineLayer.
class LineLayer::Props : public Layer::Props {
 public:
  using super = Layer::Props;

  // TODO(ilija@unfolded.ai): Should be an enum
  /// \brief Units in which the width is specified.
  std::string widthUnits{"pixels"};

  /// \brief Scale to use for line width data.
  float widthScale{1};

  /// \brief Minimum width of the line, in pixels.
  float widthMinPixels{0};

  /// \brief Maximum width of the line, in pixels.
  float widthMaxPixels{std::numeric_limits<float>::max()};

  /// Property accessors
  std::function<ArrowMapper::Vector3FloatAccessor> getSourcePosition{
      [](const Row& row) { return row.getVector3<float>("sourcePosition"); }};
  std::function<ArrowMapper::Vector3FloatAccessor> getTargetPosition{
      [](const Row& row) { return row.getVector3<float>("targetPosition"); }};
  std::function<ArrowMapper::Vector4FloatAccessor> getColor{
      [](const Row&) { return mathgl::Vector4<float>(0.0, 0.0, 0.0, 255.0); }};
  std::function<ArrowMapper::FloatAccessor> getWidth{[](const Row&) { return 1.0; }};

  // Property Type Machinery
  static constexpr const char* getTypeName() { return "LineLayer"; }
  auto getProperties() const -> const std::shared_ptr<Properties> override;
  auto makeComponent(std::shared_ptr<Component::Props> props) const -> std::shared_ptr<Component> override {
    return std::make_shared<LineLayer>(std::dynamic_pointer_cast<LineLayer::Props>(props));
  }
};

/// The order of fields in this structure is crucial for it to be mapped to its GLSL counterpart properly.
struct LineLayerUniforms {
  float opacity;
  float widthScale;
  float widthMinPixels;
  float widthMaxPixels;
};

}  // namespace deckgl

#endif  // DECKGL_LAYERS_LINE_LAYER_H
