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

#include <string>

#include "deck.gl/core.h"  // import {Layer, project32, picking} from '@deck.gl/core';

// import GL from '@luma.gl/constants';
// import {Model, Geometry} from '@luma.gl/core';

namespace deckgl {

class ScatterplotLayer : public Layer {
 public:
  using super = Layer;
  class Props;

 protected:
  void initializeState() override;
  void updateState(const ChangeFlags &, const Layer::Props *oldProps) override;
  void finalizeState() override;
  void drawState() override;

 private:
  auto _getModel(void *gl) -> std::shared_ptr<lumagl::Model>;
};

class ScatterplotLayer::Props : public Layer::Props {
 public:
  static constexpr const char *getTypeName() { return "ScatterplotLayer"; }

  bool filled;
  bool stroked;

  std::string lineWidthUnits;
  float lineWidthScale;      //
  float lineWidthMinPixels;  // min point radius in pixels
  float lineWidthMaxPixels;  // max point radius in pixels

  float radiusScale;
  float radiusMinPixels;  // min point radius in pixels
  float radiusMaxPixels;  // max point radius in pixels

  // std::function<(auto row) -> Vector3<double>> getPosition,
  // {type: 'accessor', value: x => x.position}, std::function<(auto row) -> float>
  // getRadius,
  // {type: 'accessor', value: 1}, std::function<(auto row) ->
  // ColorRGBA> getFillColor, // {type: 'accessor', value: DEFAULT_COLOR},
  // std::function<(auto row) -> ColorRGBA> getLineColor, // {type: 'accessor',
  // value: DEFAULT_COLOR}, std::function<(auto row) -> float> getLineWidth, //
  // {type: 'accessor', value: 1},

  Props()
      : filled{true},
        stroked{false},

        lineWidthUnits{"meters"},
        lineWidthScale{1},
        lineWidthMinPixels{1},
        lineWidthMaxPixels{0},

        radiusScale{1},
        radiusMinPixels{1},
        radiusMaxPixels{0} {}

  auto getProperties() const -> const Properties * override;
};

}  // namespace deckgl

#endif  // DECKGL_LAYERS_SCATTERPLOT_LAYER_H
