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

#include "./line-layer-fragment.glsl.h"
#include "./line-layer-vertex.glsl.h"

namespace deckgl {

/*
const DEFAULT_COLOR = [0, 0, 0, 255];
*/

class LineLayerProps : public LayerProps {
 public:
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
  LineLayerProps()
      : widthUnits{"pixels"}, widthScale{1}, widthMinPixels{0}, widthMaxPixels{std::numeric_limits<float>::max()} {}

 protected:
  auto getParentProps() const -> std::shared_ptr<Props> { return std::shared_ptr<Props>(new Layer::Props()); }
  auto getOwnPropTypes() const -> const std::map<const std::string, const Prop *> *;
};

class LineLayerState : public LayerState {
 public:
};

class LineLayer : public Layer {
 public:
  using Props = LineLayerProps;
};

/*
  getShaders() {
    return super.getShaders({vs, fs, modules: [project32, picking]});
  }

  initializeState() {
    const attributeManager = this->getAttributeManager();

    attributeManager->addInstanced({
      instanceSourcePositions: {
        size: 3,
        type: GL.DOUBLE,
        fp64: this->use64bitPositions(),
        transition: true,
        accessor: 'getSourcePosition'
      },
      instanceTargetPositions: {
        size: 3,
        type: GL.DOUBLE,
        fp64: this->use64bitPositions(),
        transition: true,
        accessor: 'getTargetPosition'
      },
      instanceColors: {
        size: this->props.colorFormat.length,
        type: GL.UNSIGNED_BYTE,
        normalized: true,
        transition: true,
        accessor: 'getColor',
        defaultValue: [0, 0, 0, 255]
      },
      instanceWidths: {
        size: 1,
        transition: true,
        accessor: 'getWidth',
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
    const {widthUnits, widthScale, widthMinPixels, widthMaxPixels} =
this->props;

    const widthMultiplier = widthUnits === 'pixels' ? viewport.metersPerPixel :
1;

    this->state.model
      .setUniforms(
        Object.assign({}, uniforms, {
          widthScale: widthScale * widthMultiplier,
          widthMinPixels,
          widthMaxPixels
        })
      )
      .draw();
  }

  _getModel(gl) {
    //
    //  (0, -1)-------------_(1, -1)
    //       |          _,-"  |
    //       o      _,-"      o
    //       |  _,-"          |
    //  (0, 1)"-------------(1, 1)
    //
    const positions = [0, -1, 0, 0, 1, 0, 1, -1, 0, 1, 1, 0];

    return new Model(
      gl,
      Object.assign({}, this->getShaders(), {
        id: this->props.id,
        geometry: new Geometry({
          drawMode: GL.TRIANGLE_STRIP,
          attributes: {
            positions: new Float32Array(positions)
          }
        }),
        isInstanced: true
      })
    );
  }
}

LineLayer.layerName = 'LineLayer';
LineLayer.defaultProps = defaultProps;
*/

}  // namespace deckgl

#endif  // DECKGL_LAYERS_LINE_LAYER_H
