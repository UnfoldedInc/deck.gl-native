#include "./scatterplot-layer.h"

//#include "./scatterplot-layer-fragment.glsl.h"
//#include "./scatterplot-layer-vertex.glsl.h"
#include "deck.gl/core.h"

using namespace deckgl;

const std::vector<const PropertyType*> propTypeDefs = {
    new PropertyTypeT<bool>{
        "filled", [](const Props* props) { return dynamic_cast<const ScatterplotLayer::Props*>(props)->filled; },
        [](Props* props, bool value) { return dynamic_cast<ScatterplotLayer::Props*>(props)->filled = value; }, true},
    new PropertyTypeT<bool>{
        "stroked", [](const Props* props) { return dynamic_cast<const ScatterplotLayer::Props*>(props)->stroked; },
        [](Props* props, bool value) { return dynamic_cast<ScatterplotLayer::Props*>(props)->stroked = value; }, false},
    // new PropertyTypeT<std::string>{"lineWidthUnits",
    //     [](const Props* props) { return dynamic_cast<ScatterplotLayer::Props *>(props)->widthUnits; },
    //     [](Props* props, bool value) { return dynamic_cast<ScatterplotLayer::Props
    //     *>(props)->widthUnits = value; }, true}},
    new PropertyTypeT<float>{
        "lineWidthScale",
        [](const Props* props) { return dynamic_cast<const ScatterplotLayer::Props*>(props)->lineWidthScale; },
        [](Props* props, float value) { return dynamic_cast<ScatterplotLayer::Props*>(props)->lineWidthScale = value; },
        1.0},
    new PropertyTypeT<float>{
        "lineWidthMinPixels",
        [](const Props* props) { return dynamic_cast<const ScatterplotLayer::Props*>(props)->lineWidthMinPixels; },
        [](Props* props, float value) {
          return dynamic_cast<ScatterplotLayer::Props*>(props)->lineWidthMinPixels = value;
        },
        0.0},
    new PropertyTypeT<float>{
        "lineWidthMaxPixels",
        [](const Props* props) { return dynamic_cast<const ScatterplotLayer::Props*>(props)->lineWidthMaxPixels; },
        [](Props* props, float value) {
          return dynamic_cast<ScatterplotLayer::Props*>(props)->lineWidthMaxPixels = value;
        },
        std::numeric_limits<float>::max()},
    new PropertyTypeT<float>{
        "radiusScale",
        [](const Props* props) { return dynamic_cast<const ScatterplotLayer::Props*>(props)->radiusScale; },
        [](Props* props, float value) { return dynamic_cast<ScatterplotLayer::Props*>(props)->radiusScale = value; },
        1.0},
    new PropertyTypeT<float>{
        "radiusMinPixels",
        [](const Props* props) { return dynamic_cast<const ScatterplotLayer::Props*>(props)->radiusMinPixels; },
        [](Props* props, float value) {
          return dynamic_cast<ScatterplotLayer::Props*>(props)->radiusMinPixels = value;
        },
        0.0},

    new PropertyTypeT<float>{
        "radiusMaxPixels",
        [](const Props* props) { return dynamic_cast<const ScatterplotLayer::Props*>(props)->radiusMaxPixels; },
        [](Props* props, float value) {
          return dynamic_cast<ScatterplotLayer::Props*>(props)->radiusMaxPixels = value;
        },
        std::numeric_limits<float>::max()}};

// TODO - this could be auto-injected?

auto ScatterplotLayer::Props::getPropertyTypes() const -> const PropertyTypes* {
  static PropertyTypes propTypes{PropertyTypes::from<ScatterplotLayer>("ScatterplotLayer", propTypeDefs)};
  return &propTypes;
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

void ScatterplotLayer::draw() {  // uniforms
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

auto ScatterplotLayer::_getModel(void* gl) -> std::shared_ptr<Model> {
  // a square that minimally cover the unit circle
  // const positions = [ -1, -1, 0, -1, 1, 0, 1, 1, 0, 1, -1, 0 ];

  return std::shared_ptr<Model>(new Model(gl));
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
