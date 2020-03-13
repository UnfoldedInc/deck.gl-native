#include "./line-layer.h"

#include "./line-layer-fragment.glsl.h"
#include "./line-layer-vertex.glsl.h"
#include "deck.gl/core.h"

using namespace deckgl;

const std::map<const std::string, const Prop*> propTypeMap = {
    // {"widthUnits", new PropType<LineLayer, std::string>{
    //                    [](const LineLayer::Props* props) { return props->widthUnits; },
    //                    [](LineLayer::Props* props, bool value) { return props->widthUnits = value; }, true}},
    {"widthScale", new PropType<LineLayer, float>{
                       [](const LineLayer::Props* props) { return props->widthScale; },
                       [](LineLayer::Props* props, float value) { return props->widthScale = value; }, 1.0}},
    {"widthMinPixels", new PropType<LineLayer, float>{
                           [](const LineLayer::Props* props) { return props->widthMinPixels; },
                           [](LineLayer::Props* props, float value) { return props->widthMinPixels = value; }, 0.0}},
    {"widthMaxPixels",
     new PropType<LineLayer, float>{[](const LineLayer::Props* props) { return props->widthMaxPixels; },
                                    [](LineLayer::Props* props, float value) { return props->widthMaxPixels = value; },
                                    std::numeric_limits<float>::max()}}};

auto LineLayer::Props::getPropTypes() const -> const PropTypes* {
  static PropTypes propTypes{PropTypes::from<LineLayer>(propTypeMap)};
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
