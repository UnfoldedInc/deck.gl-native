#include "./scatterplot-layer.h"

#include "deck.gl/core.h"

using namespace deckgl;

const std::map<const std::string, const Prop*> propTypes = {
    {"filled", new PropType<ScatterplotLayer, bool>{
                   [](const ScatterplotLayer::Props* props) { return props->filled; },
                   [](ScatterplotLayer::Props* props, bool value) { return props->filled = value; }, true}},
    {"stroked", new PropType<ScatterplotLayer, bool>{
                    [](const ScatterplotLayer::Props* props) { return props->stroked; },
                    [](ScatterplotLayer::Props* props, bool value) { return props->stroked = value; }, false}},

    // {"lineWidthUnits", new PropType<ScatterplotLayer, std::string>{
    //                    [](const ScatterplotLayer::Props* props) { return props->widthUnits; },
    //                    [](ScatterplotLayer::Props* props, bool value) { return props->widthUnits = value; }, true}},

    {"lineWidthScale",
     new PropType<ScatterplotLayer, float>{
         [](const ScatterplotLayer::Props* props) { return props->lineWidthScale; },
         [](ScatterplotLayer::Props* props, float value) { return props->lineWidthScale = value; }, 1.0}},
    {"lineWidthMinPixels",
     new PropType<ScatterplotLayer, float>{
         [](const ScatterplotLayer::Props* props) { return props->lineWidthMinPixels; },
         [](ScatterplotLayer::Props* props, float value) { return props->lineWidthMinPixels = value; }, 0.0}},
    {"lineWidthMaxPixels",
     new PropType<ScatterplotLayer, float>{
         [](const ScatterplotLayer::Props* props) { return props->lineWidthMaxPixels; },
         [](ScatterplotLayer::Props* props, float value) { return props->lineWidthMaxPixels = value; },
         std::numeric_limits<float>::max()}},

    {"radiusScale", new PropType<ScatterplotLayer, float>{
                        [](const ScatterplotLayer::Props* props) { return props->radiusScale; },
                        [](ScatterplotLayer::Props* props, float value) { return props->radiusScale = value; }, 1.0}},
    {"radiusMinPixels",
     new PropType<ScatterplotLayer, float>{
         [](const ScatterplotLayer::Props* props) { return props->radiusMinPixels; },
         [](ScatterplotLayer::Props* props, float value) { return props->radiusMinPixels = value; }, 0.0}},
    {"radiusMaxPixels", new PropType<ScatterplotLayer, float>{
                            [](const ScatterplotLayer::Props* props) { return props->radiusMaxPixels; },
                            [](ScatterplotLayer::Props* props, float value) { return props->radiusMaxPixels = value; },
                            std::numeric_limits<float>::max()}}};

auto ScatterplotLayer::Props::getOwnPropTypes() const -> const std::map<const std::string, const Prop*>* {
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
