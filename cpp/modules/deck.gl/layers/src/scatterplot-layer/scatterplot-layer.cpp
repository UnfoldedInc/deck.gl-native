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

#include "./scatterplot-layer.h"  // NOLINT(build/include)

// #include "./scatterplot-layer-fragment.glsl.h"
// #include "./scatterplot-layer-vertex.glsl.h"
#include "deck.gl/core.h"

using namespace deckgl;

const std::vector<const Property*> propTypeDefs = {
    new PropertyT<bool>{
        "filled", [](const JSONObject* props) { return dynamic_cast<const ScatterplotLayer::Props*>(props)->filled; },
        [](JSONObject* props, bool value) { return dynamic_cast<ScatterplotLayer::Props*>(props)->filled = value; },
        true},
    new PropertyT<bool>{
        "stroked", [](const JSONObject* props) { return dynamic_cast<const ScatterplotLayer::Props*>(props)->stroked; },
        [](JSONObject* props, bool value) { return dynamic_cast<ScatterplotLayer::Props*>(props)->stroked = value; },
        false},
    // new PropertyT<std::string>{"lineWidthUnits",
    //     [](const JSONObject* props) { return dynamic_cast<ScatterplotLayer::Props *>(props)->widthUnits; },
    //     [](JSONObject* props, bool value) { return dynamic_cast<ScatterplotLayer::Props
    //     *>(props)->widthUnits = value; }, true}},
    new PropertyT<float>{
        "lineWidthScale",
        [](const JSONObject* props) { return dynamic_cast<const ScatterplotLayer::Props*>(props)->lineWidthScale; },
        [](JSONObject* props, float value) {
          return dynamic_cast<ScatterplotLayer::Props*>(props)->lineWidthScale = value;
        },
        1.0},
    new PropertyT<float>{
        "lineWidthMinPixels",
        [](const JSONObject* props) { return dynamic_cast<const ScatterplotLayer::Props*>(props)->lineWidthMinPixels; },
        [](JSONObject* props, float value) {
          return dynamic_cast<ScatterplotLayer::Props*>(props)->lineWidthMinPixels = value;
        },
        0.0},
    new PropertyT<float>{
        "lineWidthMaxPixels",
        [](const JSONObject* props) { return dynamic_cast<const ScatterplotLayer::Props*>(props)->lineWidthMaxPixels; },
        [](JSONObject* props, float value) {
          return dynamic_cast<ScatterplotLayer::Props*>(props)->lineWidthMaxPixels = value;
        },
        std::numeric_limits<float>::max()},
    new PropertyT<float>{
        "radiusScale",
        [](const JSONObject* props) { return dynamic_cast<const ScatterplotLayer::Props*>(props)->radiusScale; },
        [](JSONObject* props, float value) {
          return dynamic_cast<ScatterplotLayer::Props*>(props)->radiusScale = value;
        },
        1.0},
    new PropertyT<float>{
        "radiusMinPixels",
        [](const JSONObject* props) { return dynamic_cast<const ScatterplotLayer::Props*>(props)->radiusMinPixels; },
        [](JSONObject* props, float value) {
          return dynamic_cast<ScatterplotLayer::Props*>(props)->radiusMinPixels = value;
        },
        0.0},

    new PropertyT<float>{
        "radiusMaxPixels",
        [](const JSONObject* props) { return dynamic_cast<const ScatterplotLayer::Props*>(props)->radiusMaxPixels; },
        [](JSONObject* props, float value) {
          return dynamic_cast<ScatterplotLayer::Props*>(props)->radiusMaxPixels = value;
        },
        std::numeric_limits<float>::max()}};

auto ScatterplotLayer::Props::getProperties() const -> const Properties* {
  static Properties properties{Properties::from<ScatterplotLayer::Props>("ScatterplotLayer", propTypeDefs)};
  return &properties;
}

/*
const DEFAULT_COLOR = [0, 0, 0, 255];

const defaultProps = {
  radiusScale: {type: 'number', min: 0, value: 1},
  radiusMinPixels: {type: 'number', min: 0, value: 0}, //  min point radius in
pixels radiusMaxPixels: {type: 'number', min: 0, value:
Number.MAX_SAFE_INTEGER}, // max point radius in pixels

  lineWidthUnits: 'meters',
  lineWidthScale: {type: 'number', min: 0, value: 1},
  lineWidthMinPixels: {type: 'number', min: 0, value: 0},
  lineWidthMaxPixels: {type: 'number', min: 0, value: Number.MAX_SAFE_INTEGER},

  stroked: false,
  filled: true,

  getPosition: {type: 'accessor', value: x => x.position},
  getRadius: {type: 'accessor', value: 1},
  getFillColor: {type: 'accessor', value: DEFAULT_COLOR},
  getLineColor: {type: 'accessor', value: DEFAULT_COLOR},
  getLineWidth: {type: 'accessor', value: 1},
};
*/

// auto ScatterplotLayer::getShaders(id) {
//   return super.getShaders({vs, fs, modules: [project32, picking]});
// }

void ScatterplotLayer::initializeState() {
  /*
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
  */
}

void ScatterplotLayer::updateState(const Layer::ChangeFlags& changeFlags, const Layer::Props* oldProps) {
  /*
  super.updateState({props, oldProps, changeFlags});
  if (changeFlags.extensionsChanged) {
    const {gl} = this->context;
    if (this->state.model) {
      this->state.model.delete();
    }
    this->setState({model: this->_getModel(gl)});
    this->getAttributeManager().invalidateAll();
  }
  */
}

void ScatterplotLayer::finalizeState() {}

void ScatterplotLayer::drawState() {  // uniforms
  /*
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
    */
}

auto ScatterplotLayer::_getModel(wgpu::Device device) -> std::shared_ptr<lumagl::Model> {
  // a square that minimally cover the unit circle
  // const positions = [ -1, -1, 0, -1, 1, 0, 1, 1, 0, 1, -1, 0 ];

  return std::shared_ptr<lumagl::Model>(new lumagl::Model(device));
  // Object.assign(this->getShaders(), {
  //   id: this->props.id,
  //   geometry: new Geometry({
  //     drawMode: GL.TRIANGLE_FAN,
  //     vertexCount: 4,
  //     attributes: {
  //       positions: {size: 3, value: new Float32Array(positions)}
  //     }
  //   }),
  //   isInstanced: true
  // })
}
