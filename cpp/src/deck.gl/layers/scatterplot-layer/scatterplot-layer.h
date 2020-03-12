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

// import vs from './scatterplot-layer-vertex.glsl';
// import fs from './scatterplot-layer-fragment.glsl';

namespace deckgl {

class ScatterplotLayerProps : public LayerProps {
 public:
  /*
    ScatterPlotLayerProps()
      : radiusScale{1}
      , radiusMinPixels{1}
      , radiusMaxPixels{0}
      , lineWidthUnits{"meters"}
      , stroked{false}
      , filled{true}
      {}

    float radiusScale, // {type: 'number', min: 0, value: 1},
    float radiusMinPixels, // {type: 'number', min: 0, value: 0}, //  min
    point radius in pixels float radiusMaxPixels, // {type: 'number', min: 0,
    value: Number.MAX_SAFE_INTEGER}, // max point radius in pixels

    std::string lineWidthUnits,
    lineWidthScale, // {type: 'number', min: 0, value: 1},
    lineWidthMinPixels, // {type: 'number', min: 0, value: 0},
    lineWidthMaxPixels, // {type: 'number', min: 0, value:
    Number.MAX_SAFE_INTEGER},

    stroked: false,
    filled: true,

    std::function<(auto row) -> Vector3<double>> getPosition, // {type:
    'accessor', value: x => x.position}, std::function<(auto row) -> float>
    getRadius, // {type: 'accessor', value: 1}, std::function<(auto row) ->
    ColorRGBA> getFillColor, // {type: 'accessor', value: DEFAULT_COLOR},
    std::function<(auto row) -> ColorRGBA> getLineColor, // {type: 'accessor',
    value: DEFAULT_COLOR}, std::function<(auto row) -> float> getLineWidth, //
    {type: 'accessor', value: 1},
    */
};

class ScatterplotLayerState : public LayerState {};

class ScatterplotLayer : public Layer {};

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
