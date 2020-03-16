#include "./line-layer.h"

#include "./line-layer-fragment.glsl.h"
#include "./line-layer-vertex.glsl.h"
#include "deck.gl/core.h"

using namespace deckgl;

const std::vector<const PropertyType*> propTypeDefs = {
    //  new PropertyTypeT<std::string>{"widthUnits",
    //      [](const LineLayer::Props* props) { return dynamic_cast<const LineLayer*>(props)->widthUnits; },
    //      [](LineLayer::Props* props, bool value) { return dynamic_cast<LineLayer*>(props)->widthUnits = value; },
    //      true},
    new PropertyTypeT<float>{
        "widthScale", [](const Props* props) { return dynamic_cast<const LineLayer::Props*>(props)->widthScale; },
        [](Props* props, float value) { return dynamic_cast<LineLayer::Props*>(props)->widthScale = value; }, 1.0},
    new PropertyTypeT<float>{
        "widthMinPixels",
        [](const Props* props) { return dynamic_cast<const LineLayer::Props*>(props)->widthMinPixels; },
        [](Props* props, float value) { return dynamic_cast<LineLayer::Props*>(props)->widthMinPixels = value; }, 0.0},
    new PropertyTypeT<float>{
        "widthMaxPixels",
        [](const Props* props) { return dynamic_cast<const LineLayer::Props*>(props)->widthMaxPixels; },
        [](Props* props, float value) { return dynamic_cast<LineLayer::Props*>(props)->widthMaxPixels = value; },
        std::numeric_limits<float>::max()}};

auto LineLayer::Props::getPropertyTypes() const -> const PropertyTypes* {
  static PropertyTypes propTypes{PropertyTypes::from<LineLayer>("LineLayer", propTypeDefs)};
  return &propTypes;
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

void LineLayer::draw() {  // {uniforms}
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

auto LineLayer::_getModel(void* gl) -> std::shared_ptr<Model> {
  return std::shared_ptr<Model>{nullptr};
  //
  //  (0, -1)-------------_(1, -1)
  //       |          _,-"  |
  //       o      _,-"      o
  //       |  _,-"          |
  //  (0, 1)"-------------(1, 1)
  //
  /*
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
  */
}
