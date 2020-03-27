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
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <optional>
#include <string>

#include "./component.h"
#include "./constants.h"
#include "./layer-context.h"
#include "attribute/attribute-manager.h"
#include "deck.gl/json.h"  // {Component, PropTypes}
#include "luma.gl/core.h"
#include "math.gl/core.h"

/* eslint-disable react/no-direct-mutation-state */
// import {COORDINATE_SYSTEM} from "./constants";
// import AttributeManager from "./attribute/attribute-manager";
// import GL from "@luma.gl/constants";
// import {withParameters, setParameters} from "@luma.gl/core";
// import {mergeShaders} from "../utils/shader";
// import {projectPosition, getWorldPosition} from "../shaderlib/project/project-functions";
// import {worldToPixels} from "@math.gl/web-mercator";
// import {load} from "@loaders.gl/core";

namespace deckgl {

// const TRACE_CHANGE_FLAG = "layer.changeFlag";
// const TRACE_INITIALIZE = "layer.initialize";
// const TRACE_UPDATE = "layer.update";
// const TRACE_FINALIZE = "layer.finalize";
// const TRACE_MATCHED = "layer.matched";

// TODO(ib@unfolded.ai): these should be imported from other files

// let pickingColorCache = new Uint8ClampedArray(0);
class ColorRGBA {
 public:
  ColorRGBA(float r_, float g_, float b_, float a_) : r{r_}, g{g_}, b{b_}, a{a_} {}
  float r, g, b, a;
};

class Layer : public Component {
 public:
  class Props;
  class ChangeFlags;

  const auto props() const { return std::dynamic_pointer_cast<const Layer::Props>(this->_props); }

  const Layer::Props* oldProps;
  const LayerContext* context;

  std::string needsRedraw;
  std::string needsUpdate;
  std::shared_ptr<AttributeManager> attributeManager;
  std::list<std::shared_ptr<lumagl::Model>> models;

  explicit Layer(std::shared_ptr<Layer::Props> props)
      : Component{std::dynamic_pointer_cast<Component::Props>(props)} {}

  // Update all props
  void setProps(std::shared_ptr<Layer::Props> newProps);

  void triggerUpdate(const std::string& attributeName);

  // auto toString() {

  // Public API

  // CHANGE MANAGEMENT

  // Sets the redraw flag for this layer, will trigger a redraw next animation frame
  void setNeedsRedraw(const std::string& reason);

  // This layer needs a deep update
  void setNeedsUpdate(const std::string& reason);

  // Checks state of attributes and model
  auto getNeedsRedraw(bool clearRedrawFlags = false) -> std::optional<std::string>;

  // Checks if layer attributes needs updating
  auto getNeedsUpdate() -> std::optional<std::string>;

  // CHANGE MANAGEMENT
  void setDataChangedFlag(const std::string& reason);
  void setPropsChangedFlag(const std::string& reason);
  void setViewportChangedFlag(const std::string& reason);
  auto getChangeFlags() -> ChangeFlags;
  void clearChangeFlags();
  void _updateChangeFlags();

  // Returns true if the layer is pickable and visible.
  auto isPickable() const -> bool;

  auto getAttributeManager() -> std::shared_ptr<AttributeManager>;

  // Return an array of models used by this layer, can be overriden by layer
  // subclass
  auto getModels() -> std::list<std::shared_ptr<lumagl::Model>>;

  // PROJECTION METHODS

  // Projects a point with current map state (lat, lon, zoom, pitch, bearing)
  // From the current layer"s coordinate system to screen
  /*
  project(xyz)
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
  encodePickingColor(i, target = []);

  // Returns the index corresponding to a picking color that doesn"t match any subfeature
  decodePickingColor(color);
  */
  // protected:
  // LIFECYCLE METHODS - redefined by subclasses

  // Called once to set up the initial state: App can create WebGL resources
  virtual void initializeState();

  // Check if update cycle should run. Default returns changeFlags.propsOrDataChanged.
  virtual auto shouldUpdateState(const ChangeFlags&, const Layer::Props* oldProps) -> const std::optional<std::string>&;

  // Default implementation: all attributes will be invalidated and updated when data changes
  virtual void updateState(const ChangeFlags&, const Layer::Props* oldProps);

  // Called once when layer is no longer matched and state will be discarded: App can destroy WebGL resources here
  virtual void finalizeState();

  // If state has a model, draw it with supplied uniforms
  virtual void drawState();

 protected:
  // INTERNAL METHODS

  // Default implementation of attribute invalidation, can be redefined
  void invalidateAttribute(const std::string& name = "all", const std::string& diffReason = "");

  // void updateAttributes(changedAttributes) {

  // Calls attribute manager to update any WebGL attributes
  void _updateAttributes();

  // Update attribute transitions. This is called in drawLayer, no model
  // updates required.
  void _updateAttributeTransition();

  // Update uniform (prop) transitions. This is called in updateState, may
  // result in model updates.
  // void _updateUniformTransition() { const

  // Deduces numer of instances.
  auto getNumInstances() -> int;

  // Buffer layout describes how many attribute values are packed for each
  // data object The default (null) is one value each object. Some data
  // formats (e.g. paths, polygons) have various length. Their buffer layout
  //  is in the form of [L0, L1, L2, ...]
  auto getStartIndices();

  // Common code for _initialize and _update
  // void _updateState();

  /*
  void calculateInstancePickingColors(attribute, {numInstances}) {
  _setModelAttributes(model, changedAttributes) {

  // Sets the specified instanced picking color to null picking color. Used
  // for multi picking.
  void clearPickingColor(color)
  void restorePickingColors()
  */

  /*
  // Compares the layers props with old props from a matched older layer
  // and extracts change flags that describe what has change so that state
  // can be update correctly with minimal effort
  // diffProps(newProps, oldProps) {

  // Called by layer manager to validate props (in development)
  void setModuleParameters(); // moduleParameters

  // PRIVATE METHODS
  // void _updateModules({props, oldProps}) {

  void _getUpdateParams();

  // Checks state of attributes and model
  void _getNeedsRedraw();

  // Create new attribute manager
  void _getAttributeManager();

  void _onAsyncPropUpdated();
  */

 private:
  // LAYER MANAGER API (Should only be called by the deck.gl LayerManager class)
  friend class LayerManager;

  // Called by layer manager when a new layer is found
  void initialize(const LayerContext*);

  // if this layer is new (not matched with an existing layer) oldProps will be empty object
  void update();

  // Called by manager when layer is about to be disposed
  // Note: not guaranteed to be called on application shutdown
  void finalize();

  void draw();  // {moduleParameters = null, uniforms = {}, parameters = {}});

  // Helpers
  void _initState();
  void _updateState();

 public:
  // PropType diffing results
  class ChangeFlags {
   public:
    // Primary changeFlags, can be strings stating reason for change
    std::optional<std::string> dataChanged;
    std::optional<std::string> propsChanged;
    std::optional<std::string> viewportChanged;
    // std::optional<std::string> updateTriggersChanged;
    // std::optional<std::string> stateChanged;
    // std::optional<std::string> extensionsChanged;

    // Derived changeFlags
    std::optional<std::string> propsOrDataChanged;
    std::optional<std::string> somethingChanged;

    ChangeFlags()
        : dataChanged{std::nullopt},
          propsChanged{std::nullopt},
          viewportChanged{std::nullopt},
          // updateTriggersChanged{std::nullopt},
          // stateChanged{std::nullopt},
          // extensionsChanged{std::nullopt},

          // Derived changeFlags
          propsOrDataChanged{std::nullopt},
          somethingChanged{std::nullopt} {}
  };

 private:
  ChangeFlags _changeFlags;
};

class Layer::Props : public Component::Props {
 public:
  using super = Component::Props;
  static constexpr const char* getTypeName() { return "Layer"; }

  std::string id;

  std::shared_ptr<arrow::Table> data;
  std::function<auto(void*, void*)->bool> dataComparator;
  // updateTriggers // TODO - how do we handle these in C++?

  bool visible{true};
  bool pickable{false};
  float opacity{1.0};

  COORDINATE_SYSTEM coordinateSystem{COORDINATE_SYSTEM::DEFAULT};
  mathgl::Vector3<double> coordinateOrigin;
  mathgl::Matrix4<double> modelMatrix;
  bool wrapLongitude{false};

  std::string positionFormat{"XYZ"};
  std::string colorFormat{"RGBA"};

  // Offset depth based on layer index to avoid z-fighting. Negative values
  // pull layer towards the camera
  // std::function getPolygonOffset;

  // INTERACTIVITY PROPS

  // TODO(ib) - interactivity is not planned for the v0.1 prototype

  // Selection/Highlighting
  bool autoHighlight;
  ColorRGBA highlightColor{0, 0, 128, 128};
  int highlightedObjectIndex{false};

  // std::function<void()> onHover;
  // std::function<void()> onClick;
  // std::function<void()> onDragStart;
  // std::function<void()> onDrag;
  // std::function<void()> onDragEnd;

  // Property Type Machinery
  auto getProperties() const -> const Properties* override;
  auto makeComponent(std::shared_ptr<Component::Props> props) const -> Layer* override {
    return new Layer{std::dynamic_pointer_cast<Layer::Props>(props)};
  }
};

}  // namespace deckgl

#endif  // DECKGL_CORE_LAYER_H
