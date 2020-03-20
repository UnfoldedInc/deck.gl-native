#include "./layer.h"

#include "./layer-manager.h"

using namespace mathgl;
using namespace deckgl;

// Setters and getters for properties
// TODO(ib): auto generate from language-independent prop definition schema

static const std::vector<const Property*> propTypeDefs = {
    new PropertyT<bool>{
        "visible", [](const JSONObject* props) { return dynamic_cast<const Layer::Props*>(props)->visible; },
        [](JSONObject* props, bool value) { return dynamic_cast<Layer::Props*>(props)->visible = value; }, true},
    new PropertyT<bool>{
        "pickable", [](const JSONObject* props) { return dynamic_cast<const Layer::Props*>(props)->pickable; },
        [](JSONObject* props, bool value) { return dynamic_cast<Layer::Props*>(props)->pickable = value; }, false},
    new PropertyT<float>{
        "opacity", [](const JSONObject* props) { return dynamic_cast<const Layer::Props*>(props)->opacity; },
        [](JSONObject* props, float value) { return dynamic_cast<Layer::Props*>(props)->opacity = value; }, 1.0},
    new PropertyT<COORDINATE_SYSTEM>{
        "coordinateSystem",
        [](const JSONObject* props) { return dynamic_cast<const Layer::Props*>(props)->coordinateSystem; },
        [](JSONObject* props, COORDINATE_SYSTEM value) {
          return dynamic_cast<Layer::Props*>(props)->coordinateSystem = value;
        },
        COORDINATE_SYSTEM::DEFAULT},
    new PropertyT<Vector3<double>>{
        "coordinateOrigin",
        [](const JSONObject* props) { return dynamic_cast<const Layer::Props*>(props)->coordinateOrigin; },
        [](JSONObject* props, Vector3<double> value) {
          return dynamic_cast<Layer::Props*>(props)->coordinateOrigin = value;
        },
        Vector3<double>()},
    new PropertyT<Matrix4<double>>{
        "modelMatrix", [](const JSONObject* props) { return dynamic_cast<const Layer::Props*>(props)->modelMatrix; },
        [](JSONObject* props, Matrix4<double> value) {
          return dynamic_cast<Layer::Props*>(props)->modelMatrix = value;
        },
        Matrix4<double>()},
    new PropertyT<bool>{
        "wrapLongitude",
        [](const JSONObject* props) { return dynamic_cast<const Layer::Props*>(props)->wrapLongitude; },
        [](JSONObject* props, bool value) { return dynamic_cast<Layer::Props*>(props)->wrapLongitude = value; },
        false}};

auto Layer::Props::getProperties() const -> const Properties* {
  static Properties properties{Properties::from<Layer::Props>("Layer", propTypeDefs)};
  return &properties;
}

/*
class LayerProperties {
  // data: Special handling for null, see below
  // data: {type: 'data', value: EMPTY_ARRAY, async: true},
  // dataComparator: null,
  // _dataDiff: {type: 'function', value: data => data && data.__diff, compare: false, optional: true},
  // dataTransform: {type: 'function', value: null, compare: false, optional: true},
  // onDataLoad: {type: 'function', value: null, compare: false, optional: true},
  // fetch: {
  //   type: 'function',
  //   value: (url, {layer}) => load(url, layer.getLoadOptions()),
  //   compare: false
  // },
  // updateTriggers: {}, // Update triggers: a core change detection mechanism in deck.gl

  opacity: {type: 'number', min: 0, max: 1, value: 1},

  onHover: {type: 'function', value: null, compare: false, optional: true},
  onClick: {type: 'function', value: null, compare: false, optional: true},
  onDragStart: {type: 'function', value: null, compare: false, optional: true},
  onDrag: {type: 'function', value: null, compare: false, optional: true},
  onDragEnd: {type: 'function', value: null, compare: false, optional: true},

  coordinateSystem: COORDINATE_SYSTEM.DEFAULT,
  coordinateOrigin: {type: 'array', value: [0, 0, 0], compare: true},
  modelMatrix: {type: 'array', value: null, compare: true, optional: true},
  wrapLongitude: false,
  positionFormat: 'XYZ',
  colorFormat: 'RGBA',

  parameters: {},
  uniforms: {},
  extensions: [],

  // Offset depth based on layer index to avoid z-fighting.
  // Negative values pull layer towards the camera
  // https://www.opengl.org/archives/resources/faq/technical/polygonoffset.htm
  getPolygonOffset: {
    type: 'function',
    value: ({layerIndex}) => [0, -layerIndex * 100],
    compare: false
  },

  // Selection/Highlighting
  highlightedObjectIndex: null,
  autoHighlight: false,
  highlightColor: {type: 'color', value: [0, 0, 128, 128]}
};
*/

// auto toString() {
//   const className = this->constructor.layerName ||
//   this->constructor.name; return `${className}({id:
//   "${this->props.id}"})`;
// }

// Public API

// Updates selected state members and marks the object for redraw
// auto setState(updateObject) -> void {
//   this->setChangeFlags({stateChanged: true});
//   Object.assign(this->state, updateObject);
//   this->setNeedsRedraw();
// }

void Layer::setProps(std::shared_ptr<Layer::Props> newProps) {
  // this->props = newProps;
  this->setNeedsUpdate("Props updated");
  this->setNeedsRedraw("Props updated");
}

void Layer::triggerUpdate(const std::string& gpuColumnName) {
  this->attributeManager->invalidate(gpuColumnName);
  this->setNeedsUpdate(gpuColumnName);
}

// Sets the redraw flag for this layer, will trigger a redraw next animation frame
void Layer::setNeedsRedraw(const std::string& reason) { this->needsRedraw = reason; }

// This layer needs a deep update
void Layer::setNeedsUpdate(const std::string& reason) {
  this->context->layerManager->setNeedsUpdate(reason);
  this->needsUpdate = reason;
}

// Checks state of attributes and model
auto Layer::getNeedsRedraw(bool clearRedrawFlags) -> std::optional<std::string> {
  return "not implemented";  // this->_getNeedsRedraw(clearRedrawFlags);
}

// Checks if layer attributes needs updating
auto Layer::getNeedsUpdate() -> std::optional<std::string> {
  return "not implemented";
  //   // Call subclass lifecycle method
  //   return (
  //     this->internalState->needsUpdate ||
  //     this->hasUniformTransition() ||
  //     this->shouldUpdateState(this->_getUpdateParams())
  //   );
  //   // End lifecycle method
}

// Returns true if the layer is pickable and visible.
auto Layer::isPickable() const -> bool { return this->props->pickable && this->props->visible; }

// auto Layer::getAttributeManager() -> std::shared_ptr<AttributeManager> { return this->attributeManager; }

// Return an array of models used by this layer, can be overriden by layer
// subclass
auto Layer::getModels() -> std::list<std::shared_ptr<lumagl::Model>> {
  return this->models;
  // return this->state && (this->state.models || (this->state.model ?
  // [this->state.model] : []));
}

// Returns the most recent layer that matched to this state
// (When reacting to an async event, this layer may no longer be the latest)
// getCurrentLayer() {
//   return this->internalState && this->internalState->layer;
// }

// Returns the default parse options for async props
// getLoadOptions() {
//   return this->props->loadOptions;
// }

// PROJECTION METHODS

// Projects a point with current map state (lat, lon, zoom, pitch, bearing)
// From the current layer"s coordinate system to screen
/*
project(xyz) {
  const {viewport} = this->context;
  const worldPosition = getWorldPosition(xyz, {
    viewport,
    modelMatrix: this->props.modelMatrix,
    coordinateOrigin: this->props.coordinateOrigin,
    coordinateSystem: this->props.coordinateSystem
  });
  const [x, y, z] = worldToPixels(worldPosition,
viewport.pixelProjectionMatrix); return xyz.length == 2 ? [x, y] : [x, y,
z];
}

// Note: this does not reverse `project`.
// Always unprojects to the viewport"s coordinate system
unproject(xy) {
  const {viewport} = this->context;
  return viewport.unproject(xy);
}

projectPosition(xyz) {
  return projectPosition(xyz, {
    viewport: this->context->viewport,
    modelMatrix: this->props.modelMatrix,
    coordinateOrigin: this->props.coordinateOrigin,
    coordinateSystem: this->props.coordinateSystem
  });
}

use64bitPositions() {
  const {coordinateSystem} = this->props;
  return (
    coordinateSystem == COORDINATE_SYSTEM.DEFAULT ||
    coordinateSystem == COORDINATE_SYSTEM.LNGLAT ||
    coordinateSystem == COORDINATE_SYSTEM.CARTESIAN
  );
}

// Event handling
onHover(info, pickingEvent) {
  if (this->props.onHover) {
    return this->props.onHover(info, pickingEvent);
  }
  return false;
}

onClick(info, pickingEvent) {
  if (this->props.onClick) {
    return this->props.onClick(info, pickingEvent);
  }
  return false;
}


// Returns the picking color that doesn"t match any subfeature
// Use if some graphics do not belong to any pickable subfeature
// @return {Array} - a black color
nullPickingColor() {
  return [0, 0, 0];
}

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

// //////////////////////////////////////////////////
// LIFECYCLE METHODS, overridden by the layer subclasses

// getShaders(shaders) {
//   for (const extension of this->props.extensions) {
//     shaders = mergeShaders(shaders, extension.getShaders.call(this,
//     extension));
//   }
//   return shaders;
// }

// Let"s layer control if updateState should be called
auto Layer::shouldUpdateState(const Layer::ChangeFlags& changeFlags, const Layer::Props* oldProps)
    -> const std::optional<std::string>& {
  return changeFlags.propsOrDataChanged;
}

// Default implementation, all attributes will be invalidated and updated
// when data changes
void Layer::updateState(const Layer::ChangeFlags& changeFlags, const Layer::Props* oldProps) {
  /*
  const auto attributeManager = this->getAttributeManager();
  if (changeFlags.dataChanged && attributeManager) {
    const dataChanged = changeFlags.dataChanged;
    if (Array.isArray(dataChanged)) {
      // is partial update
      for (const dataRange of dataChanged) {
        attributeManager->invalidateAll(dataRange);
      }
    } else {
      attributeManager->invalidateAll();
    }
  }
  */
}

// Called once when layer is no longer matched and state will be discarded
// App can destroy WebGL resources here
void Layer::finalizeState() {}

// If state has a model, draw it with supplied uniforms
void Layer::drawState() {
  for (const auto model : this->getModels()) {
    model->draw();
  }
}

// called to populate the info object that is passed to the event handler
// @return null to cancel event
// getPickingInfo({info, mode}) {
//   const {index} = info;

//   if (index >= 0) {
//     // If props.data is an indexable array, get the object
//     if (Array.isArray(this->props.data)) {
//       info.object = this->props.data[index];
//     }
//   }

//   return info;
// }

// END LIFECYCLE METHODS
// //////////////////////////////////////////////////

// INTERNAL METHODS

// Default implementation of attribute invalidation, can be redefined
void Layer::invalidateAttribute(const std::string& name, const std::string& diffReason) {
  if (name == "all") {
    this->attributeManager->invalidateAll();
  } else {
    this->attributeManager->invalidate(name);
  }
}

// void updateAttributes(changedAttributes) {
//   for (const model of this->getModels()) {
//     model.setAttributes(shaderAttributes);
//   }
// }

// LAYER MANAGER API
// Should only be called by the deck.gl LayerManager class

// Called by layer manager when a new layer is found
void Layer::initialize(const LayerContext* context) {
  // debug(TRACE_INITIALIZE, this);

  this->context = context;

  // Call subclass lifecycle method
  this->initializeState();
  // End subclass lifecycle method

  auto reason = "Layer initialization";
  this->setDataChangedFlag(reason);
  this->setPropsChangedFlag(reason);
  this->setViewportChangedFlag(reason);

  this->_updateState();
}

// Called by layer manager
// if this layer is new (not matched with an existing layer) oldProps will be empty object
void Layer::update() {
  // Call subclass lifecycle method
  auto stateNeedsUpdate = this->getNeedsUpdate();
  // End lifecycle method

  // debug(TRACE_UPDATE, this, stateNeedsUpdate);

  if (stateNeedsUpdate) {
    this->_updateState();
  }
}

// Common code for _initialize and _update
void Layer::_updateState() {
  // Safely call subclass lifecycle methods
  if (!this->context->gl) {
    return;
  }

  this->updateState(this->_changeFlags, this->oldProps);
  // End subclass lifecycle methods

  // if (this->isComposite) {
  //   // Render or update previously rendered sublayers
  //   this->_renderLayers();
  // } else {

  this->setNeedsRedraw("Update state");
  // Add any subclass attributes
  this->_updateAttributes();

  // this->props = currentProps;
  // this->oldProps = nullptr;
  this->clearChangeFlags();
  this->needsUpdate = false;
  this->needsRedraw = true;
}

// Called by manager when layer is about to be disposed
// Note: not guaranteed to be called on application shutdown
void Layer::finalize() {
  // debug(TRACE_FINALIZE, this);

  // Call subclass lifecycle method
  this->finalizeState();
  // End subclass lifecycle method
}

// Calculates uniforms
void Layer::draw() {
  // Call subclass lifecycle method
  this->drawState();
  // End lifecycle method
}

// Helper methods
auto Layer::getChangeFlags() -> Layer::ChangeFlags { return this->_changeFlags; }

void Layer::setDataChangedFlag(const std::string& reason) {
  if (!this->_changeFlags.dataChanged) {
    this->_changeFlags.dataChanged = reason;
  }
  this->_updateChangeFlags();
}

// Dirty some change flags, will be handled by updateLayer
void Layer::setPropsChangedFlag(const std::string& reason) {
  if (!this->_changeFlags.dataChanged) {
    this->_changeFlags.dataChanged = reason;
  }
  this->_updateChangeFlags();
}

// Dirty some change flags, will be handled by updateLayer
void Layer::setViewportChangedFlag(const std::string& reason) {
  if (!this->_changeFlags.dataChanged) {
    this->_changeFlags.dataChanged = reason;
  }
  this->_updateChangeFlags();
}

// Clear all changeFlags, typically after an update
void Layer::clearChangeFlags() {
  // Primary changeFlags, can be strings stating reason for change
  this->_changeFlags.dataChanged = std::nullopt;
  this->_changeFlags.propsChanged = std::nullopt;
  this->_changeFlags.viewportChanged = std::nullopt;
  // this->_changeFlags.updateTriggersChanged = std::nullopt;
  // this->_changeFlags.stateChanged = std::nullopt;
  // this->_changeFlags.extensionsChanged = std::nullopt;

  this->_changeFlags.propsOrDataChanged = std::nullopt;
  this->_changeFlags.somethingChanged = std::nullopt;
}

void Layer::_updateChangeFlags() {
  // Update composite flags
  if (!this->_changeFlags.propsOrDataChanged) {
    if (this->_changeFlags.dataChanged) {
      this->_changeFlags.propsOrDataChanged = this->_changeFlags.dataChanged;
    } else if (this->_changeFlags.propsChanged) {
      this->_changeFlags.propsOrDataChanged = this->_changeFlags.propsChanged;
    }

    // changeFlags.updateTriggersChanged
    // changeFlags.extensionsChanged;
  }

  if (!this->_changeFlags.propsOrDataChanged) {
    if (this->_changeFlags.propsOrDataChanged) {
      this->_changeFlags.propsOrDataChanged = this->_changeFlags.dataChanged;
    } else if (this->_changeFlags.viewportChanged) {
      this->_changeFlags.propsOrDataChanged = this->_changeFlags.viewportChanged;
    }
    // flags.stateChanged
  }
}

// Calls attribute manager to update any WebGL attributes
void Layer::_updateAttributes() {
  /*
    const auto attributeManager = this->getAttributeManager();
    if (!attributeManager) {
      return;
    }

    // Figure out data length
    auto numInstances = this->getNumInstances(props);
    auto startIndices = 0; this->getStartIndices(props);

    attributeManager->update({
      data: props.data,
      numInstances,
      startIndices,
      transitions: props.transitions,
      buffers: props.data.attributes,
      context: this,
      // Don"t worry about non-attribute props
      ignoreUnknownAttributes: true
    });

  // const auto changedAttributes =
  // attributeManager->getChangedAttributes({clearChangedFlags: true});
  // this->updateAttributes(changedAttributes);
  */
}

/*

// Compares the layers props with old props from a matched older layer
// and extracts change flags that describe what has change so that state
// can be update correctly with minimal effort
diffProps(newProps, oldProps) {
  const changeFlags = diffProps(newProps, oldProps);

  // iterate over changedTriggers
  if (changeFlags.updateTriggersChanged) {
    for (const key in changeFlags.updateTriggersChanged) {
      if (changeFlags.updateTriggersChanged[key]) {
        this->invalidateAttribute(key);
      }
    }
  }

  // trigger uniform transitions
  if (changeFlags.transitionsChanged) {
    for (const key in changeFlags.transitionsChanged) {
      // prop changed and transition is enabled
      this->internalState->uniformTransitions.add(key, oldProps[key], newProps[key], newProps.transitions[key]);
    }
  }

  return this->setChangeFlags(changeFlags);
}

// Called by layer manager to validate props (in development)
validateProps() { validateProps(this->props); }

setModuleParameters(moduleParameters) {
  for (const model of this->getModels()) {
    model.updateModuleSettings(moduleParameters);
  }
}

// PRIVATE METHODS
_updateModules({props, oldProps}) {
  // Picking module parameters
  const {autoHighlight, highlightedObjectIndex, highlightColor} = props;
  if (oldProps.autoHighlight != = autoHighlight || oldProps.highlightedObjectIndex !=
      = highlightedObjectIndex || oldProps.highlightColor != = highlightColor) {
    const parameters = {};
    if (!autoHighlight) {
      parameters.pickingSelectedColor = null;
    }
    // TODO - fix in luma?
    highlightColor[3] = highlightColor[3] || 255;
    parameters.pickingHighlightColor = highlightColor;

    // highlightedObjectIndex will overwrite any settings from auto
    highlighting.if (Number.isInteger(highlightedObjectIndex)) {
      parameters.pickingSelectedColor =
          highlightedObjectIndex >= 0 ? this->encodePickingColor(highlightedObjectIndex) : null;
    }

    this->setModuleParameters(parameters);
  }
}

_getUpdateParams() {
  return {
    props : this->props,
    oldProps : this->internalState->getOldProps(),
    context : this->context,
    changeFlags : this->internalState->changeFlags
  };
}

// Checks state of attributes and model
_getNeedsRedraw(opts) {
  // this method may be called by the render loop as soon a the layer
  // has been created, so guard against uninitialized state
  if (!this->internalState) {
    return false;
  }

  let redraw = false;
  redraw = redraw || (this->internalState->needsRedraw && this->id);
  this->internalState->needsRedraw = this->internalState->needsRedraw && !opts.clearRedrawFlags;

  // TODO - is attribute manager needed? - Model should be enough.
  const attributeManager = this->getAttributeManager();
  const attributeManagerNeedsRedraw = attributeManager && attributeManager->getNeedsRedraw(opts);
  redraw = redraw || attributeManagerNeedsRedraw;

  return redraw;
}

// Create new attribute manager
_getAttributeManager() {
  return new AttributeManager(this->context->gl,
                              {id : this->props.id, stats : this->context->stats, timeline : this->context->timeline});
}

_initState() {
  assert(!this->internalState && !this->state);
  assert(isFinite(this->props.coordinateSystem), `${this->id} : invalid coordinateSystem`);

  const attributeManager = this->_getAttributeManager();

  if (attributeManager) {
    // All instanced layers get instancePickingColors attribute by default
    // Their shaders can use it to render a picking scene
    // TODO - this slightly slows down non instanced layers
    attributeManager->addInstanced({
      instancePickingColors :
          {type : GL.UNSIGNED_BYTE, size : 3, noAlloc : true, update : this->calculateInstancePickingColors}
    });
  }

  this->internalState = new LayerState({attributeManager, layer : this});
  this->clearChangeFlags();  // populate this->internalState->changeFlags

  this->state = {};
  // for backwards compatibility with older layers
  // TODO - remove in next release
  Object.defineProperty(this->state, "attributeManager", {
    get: () => {
      log.deprecated("layer.state.attributeManager",
"layer.getAttributeManager()"); return attributeManager;
}
});

this->internalState->layer = this;
this->internalState->uniformTransitions = new UniformTransitionManager(this->context->timeline);
this->internalState->onAsyncPropUpdated = this->_onAsyncPropUpdated.bind(this);

// Ensure any async props are updated
this->internalState->setAsyncProps(this->props);
}

// Called by layer manager to transfer state from an old layer
_transferState(oldLayer) {
  debug(TRACE_MATCHED, this, this == oldLayer);

  const {state, internalState} = oldLayer;
  assert(state && internalState);

  if (this == oldLayer) {
    return;
  }

  // Move internalState
  this->internalState = internalState;
  this->internalState->layer = this;

  // Move state
  this->state = state;
  // We keep the state ref on old layers to support async actions
  // oldLayer.state = null;

  // Ensure any async props are updated
  this->internalState->setAsyncProps(this->props);

  this->diffProps(this->props, this->internalState->getOldProps());
}

_onAsyncPropUpdated() {
  this->diffProps(this->props, this->internalState->getOldProps());
  this->setNeedsUpdate();
}
*/