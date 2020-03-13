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

#ifndef DECKGL_CORE_LAYER_H
#define DECKGL_CORE_LAYER_H

#include <exception>
#include <functional>
#include <list>
#include <map>
#include <string>

#include "../lifecycle/component.h"
#include "../lifecycle/prop-types.h"
#include "constants.h"
#include "math.gl/core.h"

namespace deckgl {

using namespace mathgl;

/* eslint-disable react/no-direct-mutation-state */
// import {COORDINATE_SYSTEM} from "./constants";
// import AttributeManager from "./attribute/attribute-manager";
// import UniformTransitionManager from "./wip/uniform-transition-manager";
// import {diffProps, validateProps} from "../lifecycle/props";
// import {count} from "../utils/count";
// import log from "../utils/log";
// import debug from "../wip/debug";
// import GL from "@luma.gl/constants";
// import {withParameters, setParameters} from "@luma.gl/core";
// import assert from "../utils/assert";
// import {mergeShaders} from "../utils/shader";
// import {projectPosition, getWorldPosition} from
// "../shaderlib/project/project-functions"; import typedArrayManager from
// "../utils/typed-array-manager";

// import Component from "../lifecycle/component";
// import LayerState from "./layer-state";

// import {worldToPixels} from "@math.gl/web-mercator";

// import {load} from "@loaders.gl/core";

// const TRACE_CHANGE_FLAG = "layer.changeFlag";
// const TRACE_INITIALIZE = "layer.initialize";
// const TRACE_UPDATE = "layer.update";
// const TRACE_FINALIZE = "layer.finalize";
// const TRACE_MATCHED = "layer.matched";

// const EMPTY_ARRAY = Object.freeze([]);

// TODO - these should be imported from other files

class Model {
 public:
  Model(void* gl) {}
  void draw() {}
};

class AttributeManager {
 public:
  void invalidate(const std::string&) {}
  void invalidateAll() {}
};

// let pickingColorCache = new Uint8ClampedArray(0);
class ColorRGBA {
 public:
  ColorRGBA(float r_, float g_, float b_, float a_) : r{r_}, g{g_}, b{b_}, a{a_} {}
  float r, g, b, a;
};

class Layer : public Component {  // : public Component
  using super = Component;

 public:
  class Props;
  class State;
  class Context;
  class ChangeFlags;

  Layer::Props* props;
  Layer::State* state;
  Layer::State* internalState;
  Layer::Context* context;

  Layer(Layer::Props* props_, Layer::State* state_ = nullptr) : props{props_}, state{state_} {}

  // Called once to set up the initial state: App can create WebGL resources
  virtual void initializeState() = 0;

  // Check if update cycle should run. Default returns changeFlags.propsOrDataChanged.
  virtual auto shouldUpdateState(const ChangeFlags&, const Layer::Props* oldProps) -> bool;

  // Default implementation: all attributes will be invalidated and updated when data changes
  virtual void updateState(const ChangeFlags&, const Layer::Props* oldProps);

  // Called once when layer is no longer matched and state will be discarded: App can destroy WebGL resources here
  virtual void finalizeState();

  // If state has a model, draw it with supplied uniforms
  virtual void draw();

  // auto toString() {

  // Public API

  // Updates selected state members and marks the object for redraw
  // auto setState(updateObject) -> void {

  // Sets the redraw flag for this layer, will trigger a redraw next animation
  // frame
  void setNeedsRedraw(bool redraw = true);

  // This layer needs a deep update
  void setNeedsUpdate();

  // Checks state of attributes and model
  // auto getNeedsRedraw(opts = {clearRedrawFlags: false}) -> bool {

  // Checks if layer attributes needs updating
  // auto needsUpdate() -> bool {

  // Returns true if the layer is pickable and visible.
  auto isPickable() const -> bool;

  auto getAttributeManager() -> AttributeManager*;

  // Return an array of models used by this layer, can be overriden by layer
  // subclass
  auto getModels() -> std::list<Model*>;

  // Returns the most recent layer that matched to this state
  // (When reacting to an async event, this layer may no longer be the latest)
  // getCurrentLayer() {

  // Returns the default parse options for async props
  // getLoadOptions() {

  // PROJECTION METHODS

  // Projects a point with current map state (lat, lon, zoom, pitch, bearing)
  // From the current layer"s coordinate system to screen
  /*
  project(xyz)
  // Note: this does not reverse `project`. Always unprojects to the viewport"s coordinate system
  unproject(xy);
  projectPosition(xyz);
  use64bitPositions();

  // Event handling
  onHover(info, pickingEvent);
  onClick(info, pickingEvent);

  // Returns the picking color that doesn"t match any subfeature
  // Use if some graphics do not belong to any pickable subfeature
  // @return {Array} - a black color
  nullPickingColor();

  // Returns the picking color that doesn"t match any subfeature
  // Use if some graphics do not belong to any pickable subfeature
  encodePickingColor(i, target = []) {
    target[0] = (i + 1) & 255;
    target[1] = ((i + 1) >> 8) & 255;
    target[2] = (((i + 1) >> 8) >> 8) & 255;
    return target;
  }

  // Returns the index corresponding to a picking color that doesn"t match any
  subfeature
  // @param {Uint8Array} color - color array to be decoded
  // @return {Array} - the decoded picking color
  decodePickingColor(color) {
    assert(color instanceof Uint8Array);
    const [i1, i2, i3] = color;
    // 1 was added to seperate from no selection
    const index = i1 + i2 * 256 + i3 * 65536 - 1;
    return index;
  }
  */
 protected:
  // INTERNAL METHODS

  // Default implementation of attribute invalidation, can be redefined
  void invalidateAttribute(const std::string& name = "all", const std::string& diffReason = "");

  // void updateAttributes(changedAttributes) {

  // Calls attribute manager to update any WebGL attributes
  void _updateAttributes(Layer::Props* props);

  // Update attribute transitions. This is called in drawLayer, no model
  // updates required.
  void _updateAttributeTransition();

  // Update uniform (prop) transitions. This is called in updateState, may
  // result in model updates.
  // void _updateUniformTransition() { const

  // Deduces numer of instances. Intention is to support:
  // - Explicit setting of numInstances
  // - Auto-deduction for ES6 containers that define a size member
  // - Auto-deduction for Classic Arrays via the built-in length attribute
  // - Auto-deduction via arrays
  auto getNumInstances() -> int;

  // Buffer layout describes how many attribute values are packed for each
  // data object The default (null) is one value each object. Some data
  // formats (e.g. paths, polygons) have various length. Their buffer layout
  //  is in the form of [L0, L1, L2, ...]
  auto getStartIndices();

  /*
  void calculateInstancePickingColors(attribute, {numInstances}) {
  _setModelAttributes(model, changedAttributes) {

  // Sets the specified instanced picking color to null picking color. Used
  // for multi picking.
  void clearPickingColor(color)
  void restorePickingColors()

  // LAYER MANAGER API
  // Should only be called by the deck.gl LayerManager class

  // Called by layer manager when a new layer is found
  void _initialize();

  // Called by layer manager
  // if this layer is new (not matched with an existing layer) oldProps will
  // be empty object
  void _update();

  // Common code for _initialize and _update
  void _updateState();

  // Called by manager when layer is about to be disposed
  // Note: not guaranteed to be called on application shutdown
  void _finalize();

  // Calculates uniforms
  // void drawLayer({moduleParameters = null, uniforms = {}, parameters = {}});

  // Helper methods
  // getChangeFlags()

  // Dirty some change flags, will be handled by updateLayer
  // setChangeFlags(flags) {

  // Clear all changeFlags, typically after an update
  // clearChangeFlags() {

  // Compares the layers props with old props from a matched older layer
  // and extracts change flags that describe what has change so that state
  // can be update correctly with minimal effort
  // diffProps(newProps, oldProps) {

  // Called by layer manager to validate props (in development)
  void validateProps();
  void setModuleParameters(); // moduleParameters

  // PRIVATE METHODS
  // void _updateModules({props, oldProps}) {

  void _getUpdateParams();

  // Checks state of attributes and model
  void _getNeedsRedraw();

  // Create new attribute manager
  void _getAttributeManager();

  void _initState();

  // Called by layer manager to transfer state from an old layer
  void _transferState(Layer *oldLayer);

  void _onAsyncPropUpdated();
  */
};

class Layer::Props : public deckgl::Props {
 public:
  Props()
      // TODO - how to deal with data ?
      : visible{true},
        pickable{false},
        opacity{1.0},
        coordinateSystem{COORDINATE_SYSTEM::DEFAULT},
        positionFormat{"XYZ"},
        colorFormat{"RGBA"},
        autoHighlight{false},
        highlightColor{0, 0, 128, 128},
        highlightedObjectIndex{-1}  // Callbacks
                                    // TODO - Initialize to noop (prevent bad_function_call exceptions)
                                    // , onHover
                                    // , onClick
                                    // , onDragStart
                                    // , onDrag
                                    // , onDragEnd
  {}

  // data: Special handling for null, see below
  // data: {type: "data", value: EMPTY_ARRAY, async: true},
  // dataComparator: null,
  // _dataDiff: {type: "function", value: data => data && data.__diff,
  // compare: false, optional: true}, dataTransform: {type: "function", value:
  // null, compare: false, optional: true}, onDataLoad: {type: "function",
  // value: null, compare: false, optional: true}, fetch: {
  //   type: "function",
  //   value: (url, {layer}) => load(url, layer.getLoadOptions()),
  //   compare: false
  // },
  // updateTriggers: {}, // Update triggers: a core change detection mechanism in deck.gl
  bool visible;
  bool pickable;
  float opacity;

  COORDINATE_SYSTEM coordinateSystem;
  Vector3<double> coordinateOrigin;
  Matrix4<double> modelMatrix;
  bool wrapLongitude;

  std::string positionFormat;
  std::string colorFormat;

  // Selection/Highlighting
  bool autoHighlight;
  ColorRGBA highlightColor;
  int highlightedObjectIndex;

  // Offset depth based on layer index to avoid z-fighting. Negative values
  // pull layer towards the camera std::function getPolygonOffset
  std::function<void()> onHover;
  std::function<void()> onClick;
  std::function<void()> onDragStart;
  std::function<void()> onDrag;
  std::function<void()> onDragEnd;

 protected:
  auto getParentProps() const -> std::shared_ptr<deckgl::Props> override { return nullptr; }
  auto getOwnPropTypes() const -> const std::map<const std::string, const Prop*>* override;
};

class Layer::State {
 public:  // friend class Layer;
  AttributeManager* attributeManager;
  bool needsRedraw;
  bool needsUpdate;
};

class Layer::Context {};

// Prop diffing results
class Layer::ChangeFlags {
 public:
  // Primary changeFlags, can be strings stating reason for change
  bool dataChanged;
  bool propsChanged;
  bool updateTriggersChanged;
  bool viewportChanged;
  bool stateChanged;
  bool extensionsChanged;

  // Derived changeFlags
  bool propsOrDataChanged;
  bool somethingChanged;

  ChangeFlags()
      : dataChanged{false},
        propsChanged{false},
        updateTriggersChanged{false},
        viewportChanged{false},
        stateChanged{false},
        extensionsChanged{false}

        // Derived changeFlags
        ,
        propsOrDataChanged{false},
        somethingChanged{false} {}
};

}  // namespace deckgl

#endif  // DECKGL_CORE_LAYER_H
