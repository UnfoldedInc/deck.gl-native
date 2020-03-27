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

#include "./layer-manager.h"  // NOLINT(build/include)

#include "./layer.h"

#undef PI
#include "range/v3/all.hpp"

using namespace deckgl;

LayerManager::LayerManager(
    std::shared_ptr<LayerContext> _context)  // (gl, {deck, stats, viewport = null, timeline = null} = {}) {
    : context{_context}                      // gl,
{
  // this->_needsRedraw = 'Initial render';
  // this->_needsUpdate = false;
  // this->_debug = false;
  // this->_onError = null;
}

// Method to call when the layer manager is not needed anymore.
LayerManager::~LayerManager() {
  // Finalize all layers
  for (auto layer : this->layers) {
    layer->finalize();
  }
}

// Check if a redraw is needed
auto LayerManager::needsRedraw(bool clearRedrawFlags) -> std::optional<std::string> {
  return "TODO: redraw checking not implemented, so we always redraw";

  /*
  auto redraw = this->_needsRedraw;
  if (clearRedrawFlags) {
    this->_needsRedraw = "";
  }

  // This layers list doesn't include sublayers, relying on composite
  // layers
  for (auto layer : this->layers) {
    // Call every layer to clear their flags
    auto layerNeedsRedraw = layer->getNeedsRedraw(clearRedrawFlags);
    // redraw = redraw || layerNeedsRedraw;
  }

  return redraw;
  */
}

// Check if a deep update of all layers is needed
// auto LayerManager::needsUpdate() -> std::string { return this->_needsUpdate; }

// Layers will be redrawn (in next animation frame)
void LayerManager::setNeedsRedraw(const std::string &reason) {
  if (this->_needsRedraw) {
    this->_needsRedraw = reason;
  }
}

// Layers will be updated deeply (in next animation frame)
// Potentially regenerating attributes and sub layers
void LayerManager::setNeedsUpdate(const std::string &reason) {
  if (!this->_needsUpdate) {
    this->_needsUpdate = reason;
  }
}

// Props

void LayerManager::setDebug(bool debug) {}
void LayerManager::setUserData(void *userData) {}
void LayerManager::setOnError() {}

// Layer API

// For JSON: Supply a new layer prop list, match against existing layers
void LayerManager::setLayersFromProps(const std::list<std::shared_ptr<Layer::Props>> &layerPropsList) {
  // Create a map of old layers
  std::map<std::string, Layer *> oldLayerMap;
  for (auto oldLayer : this->layers) {
    oldLayerMap[oldLayer->props()->id] = oldLayer.get();
  }

  // Update old layers or add new layers if not matched
  for (auto layerProps : layerPropsList) {
    // ranges::find(oldLayerMap, layerProps->id);
    auto matchedLayerIterator = oldLayerMap.find(layerProps->id);
    if (matchedLayerIterator != oldLayerMap.end()) {
      // If a layer with this id is present, set the props
      // TODO(ib@unfolded.ai): catch exceptions and continue
      auto matchedLayer = matchedLayerIterator->second;
      matchedLayer->setProps(layerProps);
      oldLayerMap.erase(matchedLayerIterator);
    } else {
      // TODO(ib@unfolded.ai): handle exceptions
      this->addLayer(std::shared_ptr<Layer>{layerProps->makeComponent(layerProps)});
    }
  }

  // Remove any unmatched layers
  for (const auto &unmatchedLayerId : oldLayerMap | ranges::views::keys) {
    // TODO(ib@unfolded.ai): handle exceptions
    this->removeLayer(unmatchedLayerId);
  }
}

void LayerManager::addLayer(std::shared_ptr<Layer> layer) {
  auto id = layer->props()->id;
  bool idExists = ranges::any_of(this->layers, [id](auto layer) { return layer->props()->id == id; });
  if (idExists) {
    throw std::runtime_error("Layer with id already exists");
  }
  this->layers.push_back(layer);
}

void LayerManager::removeLayer(std::shared_ptr<Layer> layer) {
  this->layers.remove_if([layer](auto layer_) { return layer_ == layer; });
}

void LayerManager::removeLayer(const std::string &id) {
  this->layers.remove_if([id](auto layer) { return layer->props()->id == id; });
}

/*
// Gets an (optionally) filtered list of layers
// auto LayerManager::getLayers(const std::list<std::string> &layerIds = std::list<std::string>{})
//     -> std::list<std::shared_ptr<Layer>>;

// auto LayerManager::findLayerById(const std::string &id) -> std::shared_ptr<Layer>;

void LayerManager::removeAllLayers() {
  // TODO(ib) - exception handling
  for (auto layer : this->layers) {
    layer->finalize();
  }
  this->layers.clear();
}
*/

// Update layers from last cycle if `setNeedsUpdate()` has been called
void LayerManager::updateLayers() {
  for (auto layer : this->layers) {
    // TODO(ib@unfolded.ai): handle exceptions
    layer->update();
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

  // TODO(ib@unfolded.ai): - For now we set layers before viewports to preserve changeFlags
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
  // TODO(ib@unfolded.ai): - something is generating state updates that cause rerender of
  // the same
  const shouldUpdate = forceUpdate || newLayers != = this->lastRenderedLayers;
  // debug(TRACE_SET_LAYERS, this, shouldUpdate, newLayers);

  if (!shouldUpdate) {
    return;
  }
  this->lastRenderedLayers = newLayers;

  // TODO(ib@unfolded.ai): array flattening needs to be done during JSON conversion?
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
    this->setNeedsRedraw("updating layers : $ { reason }");  // TODO(ib@unfolded.ai): string
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
// TODO(ib@unfolded.ai): - mark layers with exceptions as bad and remove from rendering
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

      sublayers = newLayer.isComposite && newLayer.getSubLayers();
      catch (err) {
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
