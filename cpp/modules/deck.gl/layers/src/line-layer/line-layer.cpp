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

#include "./line-layer.h"  // NOLINT(build/include)

// #include "./line-layer-fragment.glsl.h"
// #include "./line-layer-vertex.glsl.h"
#include "deck.gl/core.h"

using namespace deckgl;

const std::vector<const Property*> propTypeDefs = {
    //  new PropertyT<std::string>{"widthUnits",
    //      [](const JSONObject* props) { return dynamic_cast<const LineLayer*>(props)->widthUnits; },
    //      [](JSONObject* props, bool value) { return dynamic_cast<LineLayer*>(props)->widthUnits = value; },
    //      true},
    new PropertyT<float>{
        "widthScale", [](const JSONObject* props) { return dynamic_cast<const LineLayer::Props*>(props)->widthScale; },
        [](JSONObject* props, float value) { return dynamic_cast<LineLayer::Props*>(props)->widthScale = value; }, 1.0},
    new PropertyT<float>{
        "widthMinPixels",
        [](const JSONObject* props) { return dynamic_cast<const LineLayer::Props*>(props)->widthMinPixels; },
        [](JSONObject* props, float value) { return dynamic_cast<LineLayer::Props*>(props)->widthMinPixels = value; },
        0.0},
    new PropertyT<float>{
        "widthMaxPixels",
        [](const JSONObject* props) { return dynamic_cast<const LineLayer::Props*>(props)->widthMaxPixels; },
        [](JSONObject* props, float value) { return dynamic_cast<LineLayer::Props*>(props)->widthMaxPixels = value; },
        std::numeric_limits<float>::max()}};

auto LineLayer::Props::getProperties() const -> const Properties* {
  static Properties properties{Properties::from<LineLayer::Props>("LineLayer", propTypeDefs)};
  return &properties;
}

/*
const defaultProps = {
  getSourcePosition: {type: 'accessor', value: x => x.sourcePosition},
  getTargetPosition: {type: 'accessor', value: x => x.targetPosition},
  getColor: {type: 'accessor', value: DEFAULT_COLOR},
  getWidth: {type: 'accessor', value: 1},

  widthUnits: 'pixels',
  widthScale: {type: 'number', value: 1, min: 0},
  widthMinPixels: {type: 'number', value: 0, min: 0},
  widthMaxPixels: {type: 'number', value: Number.MAX_SAFE_INTEGER, min: 0}
};
*/

// getShaders() {
//   return super.getShaders({vs, fs, modules: [project32, picking]});
// }

void LineLayer::initializeState() {
  /*
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
  */
}

void LineLayer::updateState(const Layer::ChangeFlags& changeFlags, const Layer::Props* oldProps) {
  // super::updateState(props, oldProps, changeFlags);

  // if (changeFlags.extensionsChanged) {
  //   const {gl} = this->context;
  //   if (this->state->model) {
  //     this->state->model;
  //   }
  //   this->setState({model: this->_getModel(gl)});
  //   this->getAttributeManager().invalidateAll();
  // }
}

void LineLayer::finalizeState() {}

void LineLayer::drawState() {  // {uniforms}
  /*
  const {viewport} = this->context;
  const {widthUnits, widthScale, widthMinPixels, widthMaxPixels} = ;

  const widthMultiplier = widthUnits === 'pixels' ? viewport.metersPerPixel :

  this->state.model
    .setUniforms(
      Object.assign({}, uniforms, {
        widthScale: widthScale * widthMultiplier,
        widthMinPixels,
        widthMaxPixels
      })
    )
    .draw();
  */
}

auto LineLayer::_getModel(void* gl) -> std::shared_ptr<lumagl::Model> {
  return std::shared_ptr<lumagl::Model>{nullptr};
  //
  //  (0, -1)-------------_(1, -1)
  //       |          _,-"  |
  //       o      _,-"      o
  //       |  _,-"          |
  //  (0, 1)"-------------(1, 1)
  //
  /*
  const positions = [0, -1, 0, 0, 1, 0, 1, -1, 0, 1, 1, 0];

  return new lumagl::Model(
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
  */
}
