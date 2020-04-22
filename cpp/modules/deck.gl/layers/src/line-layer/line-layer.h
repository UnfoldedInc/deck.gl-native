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
#include "luma.gl/webgpu.h"

namespace deckgl {

class LineLayer : public Layer {
 public:
  class Props;
  explicit LineLayer(std::shared_ptr<LineLayer::Props> props) : Layer{std::dynamic_pointer_cast<Layer::Props>(props)} {}
  auto props() { return std::dynamic_pointer_cast<Layer::Props>(this->_props); }

  // TODO(ilija@unfolded.ai): These should be protected. Figure out how to test them without polluting with friend
  // classes
  auto getSourcePositionData(const std::shared_ptr<arrow::Table>& table) -> std::shared_ptr<arrow::Array>;
  auto getTargetPositionData(const std::shared_ptr<arrow::Table>& table) -> std::shared_ptr<arrow::Array>;
  auto getColorData(const std::shared_ptr<arrow::Table>& table) -> std::shared_ptr<arrow::Array>;
  auto getWidthData(const std::shared_ptr<arrow::Table>& table) -> std::shared_ptr<arrow::Array>;

 protected:
  void initializeState() override;
  void updateState(const Layer::ChangeFlags&, const Layer::Props* oldProps) override;
  void finalizeState() override;
  void _drawState(wgpu::RenderPassEncoder pass) override;

 private:
  auto _getModel(wgpu::Device) -> std::shared_ptr<lumagl::Model>;

  // TODO(ilija@unfolded.ai): Remove, not sure where this should be stored when created in _getModel
  std::shared_ptr<lumagl::Model> _model;
};

class LineLayer::Props : public Layer::Props {
 public:
  using super = Layer::Props;
  static constexpr const char* getTypeName() { return "LineLayer"; }

  // Property Type Machinery
  auto getProperties() const -> const Properties* override;
  auto makeComponent(std::shared_ptr<Component::Props> props) const -> LineLayer* override {
    return new LineLayer{std::dynamic_pointer_cast<LineLayer::Props>(props)};
  }

  std::string widthUnits{"pixels"};                         // 'pixels',
  float widthScale{1};                                      // {type: 'number', value: 1, min: 0},
  float widthMinPixels{0};                                  // {type: 'number', value: 0, min: 0},
  float widthMaxPixels{std::numeric_limits<float>::max()};  // {type: 'number', value: Number.MAX_SAFE_INTEGER, min: 0}

  /// Property accessors
  std::function<ArrowMapper::Vector3FloatAccessor> getSourcePosition{
      [](const Row& row) { return row.getFloatVector3("sourcePosition"); }};
  std::function<ArrowMapper::Vector3FloatAccessor> getTargetPosition{
      [](const Row& row) { return row.getFloatVector3("targetPosition"); }};
  std::function<ArrowMapper::Vector4FloatAccessor> getColor{
      [](const Row&) { return mathgl::Vector4<float>(0.0, 0.0, 0.0, 255.0); }};
  std::function<ArrowMapper::FloatAccessor> getWidth{[](const Row&) { return 1.0; }};
};

struct alignas(lumagl::utils::kMinDynamicBufferOffsetAlignment) LineLayerUniforms {
  float opacity;
  float widthScale;
  float widthMinPixels;
  float widthMaxPixels;
};

}  // namespace deckgl

#endif  // DECKGL_LAYERS_LINE_LAYER_H
