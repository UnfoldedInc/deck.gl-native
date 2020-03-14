#include "./layer.h"

using namespace deckgl;

// Setters and getters for properties
// TODO - auto generate from language-independent prop definition schema
// TODO - just use member pointer?

static const std::map<const std::string, const PropType*> propTypeMap = {
    {"visible",
     new PropTypeT<bool>{[](const Props* props) { return dynamic_cast<const Layer::Props*>(props)->visible; },
                         [](Props* props, bool value) { return dynamic_cast<Layer::Props*>(props)->visible = value; },
                         true}},
    {"pickable",
     new PropTypeT<bool>{[](const Props* props) { return dynamic_cast<const Layer::Props*>(props)->pickable; },
                         [](Props* props, bool value) { return dynamic_cast<Layer::Props*>(props)->pickable = value; },
                         false}},
    {"opacity",
     new PropTypeT<float>{[](const Props* props) { return dynamic_cast<const Layer::Props*>(props)->opacity; },
                          [](Props* props, float value) { return dynamic_cast<Layer::Props*>(props)->opacity = value; },
                          1.0}},
    {"coordinateSystem",
     new PropTypeT<COORDINATE_SYSTEM>{
         [](const Props* props) { return dynamic_cast<const Layer::Props*>(props)->coordinateSystem; },
         [](Props* props, COORDINATE_SYSTEM value) {
           return dynamic_cast<Layer::Props*>(props)->coordinateSystem = value;
         },
         COORDINATE_SYSTEM::DEFAULT}},
    {"coordinateOrigin",
     new PropTypeT<Vector3<double>>{
         [](const Props* props) { return dynamic_cast<const Layer::Props*>(props)->coordinateOrigin; },
         [](Props* props, Vector3<double> value) {
           return dynamic_cast<Layer::Props*>(props)->coordinateOrigin = value;
         },
         Vector3<double>()}},
    {"modelMatrix",
     new PropTypeT<Matrix4<double>>{
         [](const Props* props) { return dynamic_cast<const Layer::Props*>(props)->modelMatrix; },
         [](Props* props, Matrix4<double> value) { return dynamic_cast<Layer::Props*>(props)->modelMatrix = value; },
         Matrix4<double>()}},
    {"wrapLongitude",
     new PropTypeT<bool>{
         [](const Props* props) { return dynamic_cast<const Layer::Props*>(props)->wrapLongitude; },
         [](Props* props, bool value) { return dynamic_cast<Layer::Props*>(props)->wrapLongitude = value; }, false}}};

auto Layer::Props::getPropTypes() const -> const PropTypes* {
  static PropTypes propTypes{PropTypes::from<Layer>("Layer", propTypeMap)};
  return &propTypes;
}

/*
class LayerPropTypes {
  // data: Special handling for null, see below
  // data: {type: 'data', value: EMPTY_ARRAY, async: true},
  // dataComparator: null,
  // _dataDiff: {type: 'function', value: data => data && data.__diff, compare:
false, optional: true},
  // dataTransform: {type: 'function', value: null, compare: false, optional:
true},
  // onDataLoad: {type: 'function', value: null, compare: false, optional:
true},
  // fetch: {
  //   type: 'function',
  //   value: (url, {layer}) => load(url, layer.getLoadOptions()),
  //   compare: false
  // },
  // updateTriggers: {}, // Update triggers: a core change detection mechanism
in deck.gl

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

// Sets the redraw flag for this layer, will trigger a redraw next animation
// frame
void Layer::setNeedsRedraw(bool redraw) {
  if (this->internalState) {
    this->internalState->needsRedraw = redraw;
  }
}

// This layer needs a deep update
void Layer::setNeedsUpdate() {
  // this->context->layerManager->setNeedsUpdate(String(this));
  this->internalState->needsUpdate = true;
}

// Checks state of attributes and model
// auto getNeedsRedraw(opts = {clearRedrawFlags: false}) -> bool {
//   return this->_getNeedsRedraw(opts);
// }

// Checks if layer attributes needs updating
// auto needsUpdate() -> bool {
//   // Call subclass lifecycle method
//   return (
//     this->internalState->needsUpdate ||
//     this->hasUniformTransition() ||
//     this->shouldUpdateState(this->_getUpdateParams())
//   );
//   // End lifecycle method
// }

// Returns true if the layer is pickable and visible.
auto Layer::isPickable() const -> bool { return this->props->pickable && this->props->visible; }

auto Layer::getAttributeManager() -> AttributeManager* { return this->internalState->attributeManager; }

// Return an array of models used by this layer, can be overriden by layer
// subclass
auto Layer::getModels() -> std::list<Model*> {
  return std::list<Model*>();
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
auto Layer::shouldUpdateState(const Layer::ChangeFlags& changeFlags, const Layer::Props* oldProps) -> bool {
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
void Layer::finalizeState() {
  for (const auto model : this->getModels()) {
    delete model;
  }
  delete this->getAttributeManager();
  // TODO this->internalState->uniformTransitions.clear();
}

// If state has a model, draw it with supplied uniforms
void Layer::draw() {
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
  const auto attributeManager = this->getAttributeManager();
  if (!attributeManager) {
    return;
  }

  if (name == "all") {
    attributeManager->invalidateAll();
  } else {
    attributeManager->invalidate(name);
  }
}

// void updateAttributes(changedAttributes) {
//   for (const model of this->getModels()) {
//     this->_setModelAttributes(model, changedAttributes);
//   }
// }

// Calls attribute manager to update any WebGL attributes
// void Layer::_updateAttributes() {
//   const auto attributeManager = this->getAttributeManager();
//   if (!attributeManager) {
//     return;
//   }

// Figure out data length
// auto numInstances = this->getNumInstances(props);
// auto startIndices = this->getStartIndices(props);

/*
  attributeManager->update({
    data: props.data,
    numInstances,
    startIndices,
    props,
    transitions: props.transitions,
    buffers: props.data.attributes,
    context: this,
    // Don"t worry about non-attribute props
    ignoreUnknownAttributes: true
  });
*/

// const auto changedAttributes =
// attributeManager->getChangedAttributes({clearChangedFlags: true});
// this->updateAttributes(changedAttributes);
// }

// Update attribute transitions. This is called in drawLayer, no model
// updates required.
/*
void _updateAttributeTransition() {
  const auto attributeManager = this->getAttributeManager();
  if (attributeManager) {
    attributeManager->updateTransition();
  }
}

// Update uniform (prop) transitions. This is called in updateState, may
result in model updates. void _updateUniformTransition() { const
{uniformTransitions} = this->internalState; if (uniformTransitions.active) {
    // clone props
    const propsInTransition = uniformTransitions.update();
    const props = Object.create(this->props);
    for (const key in propsInTransition) {
      Object.defineProperty(props, key, {value: propsInTransition[key]});
    }
    return props;
  }
  return this->props;
}
*/

/*
void calculateInstancePickingColors(attribute, {numInstances}) {
  // calculateInstancePickingColors always generates the same sequence.
  // pickingColorCache saves the largest generated sequence for reuse
  const cacheSize = pickingColorCache.length / 3;

  if (cacheSize < numInstances) {
    pickingColorCache = typedArrayManager.allocate(pickingColorCache,
numInstances, { size: 3, copy: true
    });
    // If the attribute is larger than the cache, resize the cache and
populate the missing chunk const newCacheSize = pickingColorCache.length /
3; const pickingColor = [];
    // assert(newCacheSize < 16777215, "index out of picking color range");

    for (let i = cacheSize; i < newCacheSize; i++) {
      this->encodePickingColor(i, pickingColor);
      pickingColorCache[i * 3 + 0] = pickingColor[0];
      pickingColorCache[i * 3 + 1] = pickingColor[1];
      pickingColorCache[i * 3 + 2] = pickingColor[2];
    }
  }

  attribute.value = pickingColorCache.subarray(0, numInstances * 3);
}

_setModelAttributes(model, changedAttributes) {
  const attributeManager = this->getAttributeManager();
  const excludeAttributes = model.userData.excludeAttributes || {};
  const shaderAttributes = attributeManager->getShaderAttributes(
    changedAttributes,
    excludeAttributes
  );

  model.setAttributes(shaderAttributes);
}

// Sets the specified instanced picking color to null picking color. Used
for multi picking. clearPickingColor(color) { const {pickingColors,
instancePickingColors} = this->getAttributeManager().attributes; const
colors = pickingColors || instancePickingColors;

  const i = this->decodePickingColor(color);
  const start = colors.getVertexOffset(i);
  const end = colors.getVertexOffset(i + 1);

  // Fill the sub buffer with 0s
  colors.buffer.subData({
    data: new Uint8Array(end - start),
    offset: start // 1 byte per element
  });
}

restorePickingColors() {
  const {pickingColors, instancePickingColors} =
this->getAttributeManager().attributes; const colors = pickingColors ||
instancePickingColors; colors.updateSubBuffer({startOffset: 0});
}
*/

// Deduces numer of instances. Intention is to support:
// - Explicit setting of numInstances
// - Auto-deduction for ES6 containers that define a size member
// - Auto-deduction for Classic Arrays via the built-in length attribute
// - Auto-deduction via arrays
auto getNumInstances(Layer::Props* props) -> int {
  return 0;
  // props = props || this->props;

  // // First Check if app has provided an explicit value
  // if (props.numInstances >= 0) {
  //   return props.numInstances;
  // }

  // // Second check if the layer has set its own value
  // if (this->state && this->state.numInstances !== undefined) {
  //   return this->state.numInstances;
  // }

  // // Use container library to get a count for any ES6 container or
  // object return count(props.data);
}

// Buffer layout describes how many attribute values are packed for each
// data object The default (null) is one value each object. Some data
// formats (e.g. paths, polygons) have various length. Their buffer layout
//  is in the form of [L0, L1, L2, ...]
/*
getStartIndices(props) {
  props = props || this->props;

  // First Check if startIndices is provided as an explicit value
  if (props.startIndices !== undefined) {
    return props.startIndices;
  }

  // Second check if the layer has set its own value
  if (this->state && this->state.startIndices) {
    return this->state.startIndices;
  }

  return null;
}

// LAYER MANAGER API
// Should only be called by the deck.gl LayerManager class

// Called by layer manager when a new layer is found
_initialize() {
  debug(TRACE_INITIALIZE, this);

  this->_initState();

  // Call subclass lifecycle methods
  this->initializeState(this->context);
  // Initialize extensions
  for (const extension of this->props.extensions) {
    extension.initializeState.call(this, this->context, extension);
  }
  // End subclass lifecycle methods

  // initializeState callback tends to clear state
  this->setChangeFlags({
    dataChanged: true,
    propsChanged: true,
    viewportChanged: true,
    extensionsChanged: true
  });

  this->_updateState();
}

// Called by layer manager
// if this layer is new (not matched with an existing layer) oldProps will
be empty object _update() {
  // Call subclass lifecycle method
  const stateNeedsUpdate = this->needsUpdate();
  // End lifecycle method
  debug(TRACE_UPDATE, this, stateNeedsUpdate);

  if (stateNeedsUpdate) {
    this->_updateState();
  }
}

// Common code for _initialize and _update
_updateState() {
  const currentProps = this->props;
  const propsInTransition = this->_updateUniformTransition();
  this->internalState->propsInTransition = propsInTransition;
  // Overwrite this->props during update to use in-transition prop values
  this->props = propsInTransition;

  const updateParams = this->_getUpdateParams();

  // Safely call subclass lifecycle methods
  if (this->context->gl) {
    this->updateState(updateParams);
  } else {
    try {
      this->updateState(updateParams);
    } catch (error) {
      // ignore error if gl context is missing
    }
  }
  // Execute extension updates
  for (const extension of this->props.extensions) {
    extension.updateState.call(this, updateParams, extension);
  }
  this->_updateModules(updateParams);
  // End subclass lifecycle methods

  if (this->isComposite) {
    // Render or update previously rendered sublayers
    this->_renderLayers(updateParams);
  } else {
    this->setNeedsRedraw();
    // Add any subclass attributes
    this->_updateAttributes(this->props);

    // Note: Automatic instance count update only works for single layers
    if (this->state.model) {
      this->state.model.setInstanceCount(this->getNumInstances());
    }
  }

  this->props = currentProps;
  this->clearChangeFlags();
  this->internalState->needsUpdate = false;
  this->internalState->resetOldProps();
}

// Called by manager when layer is about to be disposed
// Note: not guaranteed to be called on application shutdown
_finalize() {
  debug(TRACE_FINALIZE, this);
  assert(this->internalState && this->state);

  // Call subclass lifecycle method
  this->finalizeState(this->context);
  // Finalize extensions
  for (const extension of this->props.extensions) {
    extension.finalizeState.call(this, extension);
  }
}

// Calculates uniforms
drawLayer({moduleParameters = null, uniforms = {}, parameters = {}}) {
  this->_updateAttributeTransition();

  const currentProps = this->props;
  // Overwrite this->props during redraw to use in-transition prop values
  // `internalState->propsInTransition` could be missing if `updateState`
failed this->props = this->internalState->propsInTransition || currentProps;

  const {opacity} = this->props;
  // apply gamma to opacity to make it visually "linear"
  uniforms.opacity = Math.pow(opacity, 1 / 2.2);

  // TODO/ib - hack move to luma Model.draw
  if (moduleParameters) {
    this->setModuleParameters(moduleParameters);
  }

  // Apply polygon offset to avoid z-fighting
  // TODO - move to draw-layers
  const {getPolygonOffset} = this->props;
  const offsets = (getPolygonOffset && getPolygonOffset(uniforms)) || [0,
0];

  setParameters(this->context->gl, {polygonOffset: offsets});

  // Call subclass lifecycle method
  withParameters(this->context->gl, parameters, () => {
    this->draw({moduleParameters, uniforms, parameters, context:
this->context});
  });

  // End lifecycle method

  this->props = currentProps;
}

// Helper methods
getChangeFlags() {
  return this->internalState->changeFlags;
}

// Dirty some change flags, will be handled by updateLayer
setChangeFlags(flags) {
  const {changeFlags} = this->internalState;

  for (const key in changeFlags) {
    if (flags[key] && !changeFlags[key]) {
      changeFlags[key] = flags[key];
      debug(TRACE_CHANGE_FLAG, this, key, flags);
    }
  }

  // Update composite flags
  const propsOrDataChanged =
    changeFlags.dataChanged ||
    changeFlags.updateTriggersChanged ||
    changeFlags.propsChanged ||
    changeFlags.extensionsChanged;
  changeFlags.propsOrDataChanged = propsOrDataChanged;
  changeFlags.somethingChanged =
    propsOrDataChanged || flags.viewportChanged || flags.stateChanged;
}

// Clear all changeFlags, typically after an update
clearChangeFlags() {
  this->internalState->changeFlags = {
    // Primary changeFlags, can be strings stating reason for change
    dataChanged: false,
    propsChanged: false,
    updateTriggersChanged: false,
    viewportChanged: false,
    stateChanged: false,
    extensionsChanged: false,

    // Derived changeFlags
    propsOrDataChanged: false,
    somethingChanged: false
  };
}

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
      this->internalState->uniformTransitions.add(
        key,
        oldProps[key],
        newProps[key],
        newProps.transitions[key]
      );
    }
  }

  return this->setChangeFlags(changeFlags);
}

// Called by layer manager to validate props (in development)
validateProps() {
  validateProps(this->props);
}

setModuleParameters(moduleParameters) {
  for (const model of this->getModels()) {
    model.updateModuleSettings(moduleParameters);
  }
}

// PRIVATE METHODS
_updateModules({props, oldProps}) {
  // Picking module parameters
  const {autoHighlight, highlightedObjectIndex, highlightColor} = props;
  if (
    oldProps.autoHighlight !== autoHighlight ||
    oldProps.highlightedObjectIndex !== highlightedObjectIndex ||
    oldProps.highlightColor !== highlightColor
  ) {
    const parameters = {};
    if (!autoHighlight) {
      parameters.pickingSelectedColor = null;
    }
    // TODO - fix in luma?
    highlightColor[3] = highlightColor[3] || 255;
    parameters.pickingHighlightColor = highlightColor;

    // highlightedObjectIndex will overwrite any settings from auto
highlighting. if (Number.isInteger(highlightedObjectIndex)) {
      parameters.pickingSelectedColor =
        highlightedObjectIndex >= 0 ?
this->encodePickingColor(highlightedObjectIndex) : null;
    }

    this->setModuleParameters(parameters);
  }
}

_getUpdateParams() {
  return {
    props: this->props,
    oldProps: this->internalState->getOldProps(),
    context: this->context,
    changeFlags: this->internalState->changeFlags
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
  this->internalState->needsRedraw = this->internalState->needsRedraw &&
!opts.clearRedrawFlags;

  // TODO - is attribute manager needed? - Model should be enough.
  const attributeManager = this->getAttributeManager();
  const attributeManagerNeedsRedraw = attributeManager &&
attributeManager->getNeedsRedraw(opts); redraw = redraw ||
attributeManagerNeedsRedraw;

  return redraw;
}

// Create new attribute manager
_getAttributeManager() {
  return new AttributeManager(this->context->gl, {
    id: this->props.id,
    stats: this->context->stats,
    timeline: this->context->timeline
  });
}

_initState() {
  assert(!this->internalState && !this->state);
  assert(isFinite(this->props.coordinateSystem), `${this->id}: invalid
coordinateSystem`);

  const attributeManager = this->_getAttributeManager();

  if (attributeManager) {
    // All instanced layers get instancePickingColors attribute by default
    // Their shaders can use it to render a picking scene
    // TODO - this slightly slows down non instanced layers
    attributeManager->addInstanced({
      instancePickingColors: {
        type: GL.UNSIGNED_BYTE,
        size: 3,
        noAlloc: true,
        update: this->calculateInstancePickingColors
      }
    });
  }

  this->internalState = new LayerState({
    attributeManager,
    layer: this
  });
  this->clearChangeFlags(); // populate this->internalState->changeFlags

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
  this->internalState->uniformTransitions = new
UniformTransitionManager(this->context->timeline);
  this->internalState->onAsyncPropUpdated =
this->_onAsyncPropUpdated.bind(this);

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
}

Layer.layerName = "Layer";
Layer.defaultProps = defaultProps;

*/
