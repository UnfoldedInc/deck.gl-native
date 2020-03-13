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

#include "./layer-manager.h"

#include "./layer.h"

using namespace deckgl;

/*
import assert from '../utils/assert';
import {Timeline} from '@luma.gl/core';
import Layer from './layer';
import {LIFECYCLE} from '../lifecycle/constants';
import log from '../utils/log';
import debug from '../debug';
import {flatten} from '../utils/flatten';
import {Stats} from 'probe.gl';

import Viewport from '../viewports/viewport';
import {createProgramManager} from '../shaderlib';

const TRACE_SET_LAYERS = 'layerManager.setLayers';
const TRACE_ACTIVATE_VIEWPORT = 'layerManager.activateViewport';

// CONTEXT IS EXPOSED TO LAYERS
const INITIAL_CONTEXT = Object.seal({
layerManager: null,
deck: null,
gl: null,

// General resources
stats: null, // for tracking lifecycle performance

// GL Resources
shaderCache: null,
pickingFBO: null, // Screen-size framebuffer that layers can reuse

mousePosition: null,

userData: {} // Place for any custom app `context`
});

const layerName = layer => (layer instanceof Layer ? `${layer}` : !layer ?
'null' : 'invalid');
*/

LayerManager::LayerManager(Deck *deck)  // (gl, {deck, stats, viewport = null, timeline = null} = {}) {
    : context(deck, this)
// gl,
// // Enabling luma.gl Program caching using private API (_cachePrograms)
// programManager: gl && createProgramManager(gl),
// stats: stats || new Stats({id: 'deck.gl'}),
// // Make sure context.viewport is not empty on the first layer
// initialization viewport: viewport || new Viewport({id:
// 'DEFAULT-INITIAL-VIEWPORT'}), // Current viewport, exposed to layers for
// project* function timeline: timeline || new Timeline()
{
  // Currently deck.gl expects the DeckGL.layers array to be different
  // whenever React rerenders. If the same layers array is used, the
  // LayerManager's diffing algorithm will generate a fatal error and
  // break the rendering.

  // `this->lastRenderedLayers` stores the UNFILTERED layers sent
  // down to LayerManager, so that `layers` reference can be compared.
  // If it's the same across two React render calls, the diffing logic
  // will be skipped.

  // this->_needsRedraw = 'Initial render';
  this->_needsUpdate = false;
  this->_debug = false;
  // this->_onError = null;
}

// Method to call when the layer manager is not needed anymore.
LayerManager::~LayerManager() {
  // Finalize all layers
  for (auto layer : this->layers) {
    this->_finalizeLayer(layer);
  }
}

// Check if a redraw is needed
auto LayerManager::needsRedraw(bool clearRedrawFlags) -> std::string {
  return "TODO: redraw checking not implemented so we always redraw";

  // auto redraw = this->_needsRedraw;
  // if (clearRedrawFlags) {
  //   this->_needsRedraw = "";
  // }

  // // This layers list doesn't include sublayers, relying on composite
  // // layers
  // for (auto layer : this->layers) {
  //   // Call every layer to clear their flags
  //   auto layerNeedsRedraw = layer->getNeedsRedraw(clearRedrawFlags);
  //   redraw = redraw || layerNeedsRedraw;
  // }

  // return redraw;
}

// Check if a deep update of all layers is needed
auto LayerManager::needsUpdate() -> std::string { return this->_needsUpdate; }

// Layers will be redrawn (in next animation frame)
void LayerManager::setNeedsRedraw(const std::string &reason) {
  if (this->_needsRedraw.empty()) {
    this->_needsRedraw = reason;
  }
}

// Layers will be updated deeply (in next animation frame)
// Potentially regenerating attributes and sub layers
void LayerManager::setNeedsUpdate(const std::string &reason) {
  if (!this->_needsUpdate.empty()) {
    this->_needsUpdate = reason;
  }
}

// Gets an (optionally) filtered list of layers
/*
auto LayerManager::getLayers() -> std::list<Layer *> {
  // Filtering by layerId compares beginning of strings, so that sublayers
  // will be included Dependes on the convention of adding suffixes to the
  // parent's layer name
  // return layerIds ? this->layers.filter(layer = > layerIds.find(layerId = > layer->id.indexOf(layerId) == = 0))
  //                 : this->layers;
  return this->layers;
}
*/

// Set props needed for layer rendering and picking.
/*
LayerManager::setProps(props) {
  if ('debug' in props) {
    this->_debug = props.debug;
  }

  // A way for apps to add data to context that can be accessed in layers
  if ('userData' in props) {
    this->context.userData = props.userData;
  }

  // TODO - For now we set layers before viewports to preserve changeFlags
  if ('layers' in props) {
    this->setLayers(props.layers);
  }

  if ('onError' in props) {
    this->_onError = props.onError;
  }
}
*/

// Supply a new layer list, initiating sublayer generation and layer
// matching
/*
void LayerManager::setLayers(std::list<Layer *> newLayers, bool forceUpdate) {
  // TODO - something is generating state updates that cause rerender of
  // the same
  const shouldUpdate = forceUpdate || newLayers != = this->lastRenderedLayers;
  // debug(TRACE_SET_LAYERS, this, shouldUpdate, newLayers);

  if (!shouldUpdate) {
    return;
  }
  this->lastRenderedLayers = newLayers;

  // TODO array flattening needs to be done during JSON conversion?
  // newLayers = flatten(newLayers, {filter : Boolean});

  for (auto layer : newLayers) {
    layer->context = this->context;
  }

  this->_updateLayers(this->layers, newLayers);
}
*/

// Update layers from last cycle if `setNeedsUpdate()` has been called
/*
void LayerManager::updateLayers() {
  // NOTE: For now, even if only some layer has changed, we update all
  // layers to ensure that layer id maps etc remain consistent even if
  // different sublayers are rendered
  auto reason = this->needsUpdate();
  if (reason) {
    this->setNeedsRedraw("updating layers : $ { reason }");  // TODO string
    // Force a full update
    auto forceUpdate = true;
    this->setLayers(this->lastRenderedLayers, forceUpdate);
  }
}
*/

//
// PRIVATE METHODS
//

// Make a viewport "current" in layer context, updating viewportChanged
// flags
/*
LayerManager::activateViewport(viewport) {
  assert(viewport, 'LayerManager: viewport not set');

  const oldViewport = this->context.viewport;
  const viewportChanged = !oldViewport || !viewport.equals(oldViewport);

  if (viewportChanged) {
    debug(TRACE_ACTIVATE_VIEWPORT, this, viewport);

    this->context.viewport = viewport;
    const changeFlags = {viewportChanged : true};

    // Update layers states
    // Let screen space layers update their state based on viewport
    for (auto layer : this->layers) {
      layer->setChangeFlags(changeFlags);
      this->_updateLayer(layer);
    }
  }

  return this;
}
*/

// void LayerManager::_handleError(stage, error, layer) {
//   if (this->_onError) {
//     this->_onError(error, layer);
//   } else {
//     log.error(`error during ${stage} of $ { layerName(layer) }`, error)();
//   }
// }

// Match all layers, checking for caught errors
// To avoid having an exception in one layer disrupt other layers
// TODO - mark layers with exceptions as bad and remove from rendering
// cycle?
/*
void LayerManager::_updateLayers(oldLayers, newLayers) {
// Create old layer map
std::map<std::string, Layer *> oldLayerMap;
for (const oldLayer : oldLayers) {
  if (oldLayerMap[oldLayer.id]) {
    log.warn(`Multiple old layers with same id $ { layerName(oldLayer) }`)();
  } else {
    oldLayerMap[oldLayer.id] = oldLayer;
  }
}

// Allocate array for generated layers
const generatedLayers = [];

// Match sublayers
this->_updateSublayersRecursively(newLayers, oldLayerMap, generatedLayers);

// Finalize unmatched layers
this->_finalizeOldLayers(oldLayerMap);

let needsUpdate = false;
for (auto layer : generatedLayers) {
  if (layer->hasUniformTransition()) {
    needsUpdate = true;
    break;
  }
}

this->_needsUpdate = needsUpdate;
this->layers = generatedLayers;
}
*/

// Note: adds generated layers to `generatedLayers` array parameter
/*
void LayerManager::_updateSublayersRecursively(newLayers, oldLayerMap, generatedLayers) {
  for (const newLayer of newLayers) {
    newLayer.context = this->context;

    // Given a new coming layer, find its matching old layer (if any)
    const oldLayer = oldLayerMap[newLayer.id];
    if (oldLayer == = null) {
      // null, rather than undefined, means this id was originally
      // there
      log.warn(`Multiple new layers with same id $ { layerName(newLayer) }`)();
    }
    // Remove the old layer from candidates, as it has been matched with
    // this layer
    oldLayerMap[newLayer.id] = null;

    let sublayers = null;

    // We must not generate exceptions until after layer matching is
    // complete
    try {
      if (this->_debug &&oldLayer != = newLayer) {
        newLayer.validateProps();
      }

      if (!oldLayer) {
        this->_initializeLayer(newLayer);
      } else {
        this->_transferLayerState(oldLayer, newLayer);
        this->_updateLayer(newLayer);
      }
      generatedLayers.push(newLayer);

      // Call layer lifecycle method: render sublayers
      sublayers = newLayer.isComposite && newLayer.getSubLayers();
      // End layer lifecycle method: render sublayers
    } catch (err) {
      this->_handleError('matching', err,
                         newLayer);  // Record first exception
    }

    if (sublayers) {
      this->_updateSublayersRecursively(sublayers, oldLayerMap, generatedLayers);
    }
  }
}
*/

// Finalize any old layers that were not matched
/*
void LayerManager::_finalizeOldLayers(oldLayerMap) {
  // for (const layerId in oldLayerMap) {
  //   const layer = oldLayerMap[layerId];
  //   if (layer) {
  //     this->_finalizeLayer(layer);
  //   }
  // }
}
*/

// EXCEPTION SAFE LAYER ACCESS

// Initializes a single layer, calling layer methods
void LayerManager::_initializeLayer(Layer *layer) {
  // try {
  //   layer->_initialize();
  //   layer->lifecycle = LIFECYCLE.INITIALIZED;
  // } catch (err) {
  //   this->_handleError('initialization', err, layer);
  //   // TODO - what should the lifecycle state be here?
  //   // LIFECYCLE.INITIALIZATION_FAILED?
  // }
}

void LayerManager::_transferLayerState(Layer *oldLayer, Layer *newLayer) {
  // newLayer._transferState(oldLayer);
  // newLayer.lifecycle = LIFECYCLE.MATCHED;

  // if (newLayer != = oldLayer) {
  //   oldLayer.lifecycle = LIFECYCLE.AWAITING_GC;
  // }
}

// Updates a single layer, cleaning all flags
void LayerManager::_updateLayer(Layer *layer) {
  // try {
  //   layer->_update();
  // } catch (err) {
  //   this->_handleError('update', err, layer);
  // }
}

// Finalizes a single layer
void LayerManager::_finalizeLayer(Layer *layer) {
  // this->_needsRedraw = this->_needsRedraw || `finalized $ { layerName(layer) }
  // `;

  // layer->lifecycle = LIFECYCLE.AWAITING_FINALIZATION;

  // try {
  //   layer->_finalize();
  //   layer->lifecycle = LIFECYCLE.FINALIZED;
  // } catch (err) {
  //   this->_handleError('finalization', err, layer);
  // }
}
