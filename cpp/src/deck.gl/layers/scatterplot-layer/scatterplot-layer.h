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

class ScatterplotLayerProps : public LayerProps {
 public:
  bool filled;
  bool stroked;

  std::string lineWidthUnits;
  float lineWidthScale;      //
  float lineWidthMinPixels;  // min point radius in pixels
  float lineWidthMaxPixels;  // max point radius in pixels

  float radiusScale;
  float radiusMinPixels;  // min point radius in pixels
  float radiusMaxPixels;  // max point radius in pixels

  // std::function<(auto row) -> Vector3<double>> getPosition, // {type:
  // 'accessor', value: x => x.position}, std::function<(auto row) -> float>
  // getRadius, // {type: 'accessor', value: 1}, std::function<(auto row) ->
  // ColorRGBA> getFillColor, // {type: 'accessor', value: DEFAULT_COLOR},
  // std::function<(auto row) -> ColorRGBA> getLineColor, // {type: 'accessor',
  // value: DEFAULT_COLOR}, std::function<(auto row) -> float> getLineWidth, //
  // {type: 'accessor', value: 1},

  ScatterplotLayerProps()
      : filled{true},
        stroked{false},

        lineWidthUnits{"meters"},
        lineWidthScale{1},
        lineWidthMinPixels{1},
        lineWidthMaxPixels{0},

        radiusScale{1},
        radiusMinPixels{1},
        radiusMaxPixels{0} {}

 protected:
  auto getParentProps() const -> std::shared_ptr<Props> { return std::shared_ptr<Props>(new Layer::Props()); }
  auto getOwnPropTypes() const -> const std::map<const std::string, const Prop *> *;
};

class ScatterplotLayerState : public LayerState {};

class ScatterplotLayer : public Layer {
 public:
  using Props = ScatterplotLayerProps;
};

/*
class ScatterplotLayer : Layer {
  // getShaders(id) {
  //   return super.getShaders({vs, fs, modules: [project32, picking]});
  // }

  void initializeState() {
    this->getAttributeManager()->addInstanced({
      instancePositions: {
        size: 3,
        type: GL.DOUBLE,
        fp64: this->use64bitPositions(),
        transition: true,
        accessor: 'getPosition'
      },
      instanceRadius: {
        size: 1,
        transition: true,
        accessor: 'getRadius',
        defaultValue: 1
      },
      instanceFillColors: {
        size: this->props.colorFormat.length,
        transition: true,
        normalized: true,
        type: GL.UNSIGNED_BYTE,
        accessor: 'getFillColor',
        defaultValue: [0, 0, 0, 255]
      },
      instanceLineColors: {
        size: this->props.colorFormat.length,
        transition: true,
        normalized: true,
        type: GL.UNSIGNED_BYTE,
        accessor: 'getLineColor',
        defaultValue: [0, 0, 0, 255]
      },
      instanceLineWidths: {
        size: 1,
        transition: true,
        accessor: 'getLineWidth',
        defaultValue: 1
      }
    });
  }

  updateState({props, oldProps, changeFlags}) {
    super.updateState({props, oldProps, changeFlags});
    if (changeFlags.extensionsChanged) {
      const {gl} = this->context;
      if (this->state.model) {
        this->state.model.delete();
      }
      this->setState({model: this->_getModel(gl)});
      this->getAttributeManager().invalidateAll();
    }
  }

  draw({uniforms}) {
    const {viewport} = this->context;
    const {
      radiusScale,
      radiusMinPixels,
      radiusMaxPixels,
      stroked,
      filled,
      lineWidthUnits,
      lineWidthScale,
      lineWidthMinPixels,
      lineWidthMaxPixels
    } = this->props;

    const widthMultiplier = lineWidthUnits === 'pixels' ?
viewport.metersPerPixel : 1;

    this->state.model
      .setUniforms(uniforms)
      .setUniforms({
        stroked: stroked ? 1 : 0,
        filled,
        radiusScale,
        radiusMinPixels,
        radiusMaxPixels,
        lineWidthScale: lineWidthScale * widthMultiplier,
        lineWidthMinPixels,
        lineWidthMaxPixels
      })
      .draw();
  }

  _getModel(gl) {
    // a square that minimally cover the unit circle
    const positions = [-1, -1, 0, -1, 1, 0, 1, 1, 0, 1, -1, 0];

    return new Model(
      gl,
      Object.assign(this->getShaders(), {
        id: this->props.id,
        geometry: new Geometry({
          drawMode: GL.TRIANGLE_FAN,
          vertexCount: 4,
          attributes: {
            positions: {size: 3, value: new Float32Array(positions)}
          }
        }),
        isInstanced: true
      })
    );
  }
}
*/

}  // namespace deckgl

#endif  // DECKGL_LAYERS_SCATTERPLOT_LAYER_H
