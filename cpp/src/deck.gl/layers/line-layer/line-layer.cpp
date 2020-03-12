#include "./line-layer.h"

#include "deck.gl/core.h"

using namespace deckgl;

const std::map<const std::string, const Prop*> LineLayer::Props::propTypes = {
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
