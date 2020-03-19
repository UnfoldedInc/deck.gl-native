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

#include <limits>

#include "deck.gl/core.h"  // {Layer, project32, picking}
// import GL from '@luma.gl/constants';
// import {Model, Geometry} from '@luma.gl/core';

namespace deckgl {

class LineLayer : public Layer {
 public:
  using super = Layer;
  class Props;

 protected:
  void initializeState() override;
  void updateState(const Layer::ChangeFlags &, const Layer::Props *oldProps) override;
  void finalizeState() override;
  void drawState() override;

 private:
  auto _getModel(void *gl) -> std::shared_ptr<lumagl::Model>;
};

class LineLayer::Props : public Layer::Props {
 public:
  static constexpr const char *getTypeName() { return "LineLayer"; }

  std::string widthUnits;  // : 'pixels',
  float widthScale;        //  {type: 'number', value: 1, min: 0},
  float widthMinPixels;    //  {type: 'number', value: 0, min: 0},
  float widthMaxPixels;    //  {type: 'number', value: Number.MAX_SAFE_INTEGER, min: 0}

  /*
    // std::function<(auto row) -> Vector3<double>> getSourcePosition; //
    {type: 'accessor', value: x => x.sourcePosition},
    // std::function<(auto row) -> Vector3<double>> getTargetPosition; //
    {type: 'accessor', value: x => x.targetPosition},
    // std::function<(auto row) -> ColorRGBA> getColor; //  {type: 'accessor',
    value: DEFAULT_COLOR},
    // std::function<(auto row) -> float> getWidth; //  {type: 'accessor',
    value: 1},
   */

  Props() : widthUnits{"pixels"}, widthScale{1}, widthMinPixels{0}, widthMaxPixels{std::numeric_limits<float>::max()} {}

  auto getPropertyTypes() const -> const PropertyTypes * override;
};

}  // namespace deckgl

#endif  // DECKGL_LAYERS_LINE_LAYER_H
