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

#include <probe.gl/core.h>

#include "./layer.h"

using namespace deckgl;

LayerManager::LayerManager(std::shared_ptr<LayerContext> _context) : context{_context} {
  this->_needsRedraw = "Initial render";
  this->_needsUpdate = std::nullopt;
  this->_debug = false;
}

LayerManager::~LayerManager() {
  // Finalize all layers
  for (auto layer : this->layers) {
    layer->finalize();
  }
}

auto LayerManager::needsRedraw(bool clearRedrawFlags) -> std::optional<std::string> {
  auto redraw = this->_needsRedraw;
  if (clearRedrawFlags) {
    this->_needsRedraw = std::nullopt;
  }

  if (redraw) {
    return redraw;
  }

  for (auto layer : this->layers) {
    // Call every layer to clear their flags
    if (auto layerNeedsRedraw = layer->getNeedsRedraw(clearRedrawFlags)) {
      return layerNeedsRedraw;
    }
  }

  return std::nullopt;
}

void LayerManager::setNeedsRedraw(const std::string &reason) {
  if (this->_needsRedraw) {
    this->_needsRedraw = reason;
  }
}

void LayerManager::setNeedsUpdate(const std::string &reason) {
  if (!this->_needsUpdate) {
    this->_needsUpdate = reason;
  }
}

void LayerManager::setLayersFromProps(const std::list<std::shared_ptr<Layer::Props>> &layerPropsList) {
  // Create a map of old layers
  std::map<std::string, std::shared_ptr<Layer>> oldLayerMap;
  for (auto oldLayer : this->layers) {
    oldLayerMap[oldLayer->props()->id] = oldLayer;
  }

  // Update old layers or add new layers if not matched
  for (auto layerProps : layerPropsList) {
    auto matchedLayerIterator = oldLayerMap.find(layerProps->id);
    if (matchedLayerIterator != oldLayerMap.end()) {
      // If a layer with this id is present, set the props
      // TODO(ib@unfolded.ai): Catch exceptions and continue
      auto matchedLayer = matchedLayerIterator->second;
      matchedLayer->setProps(layerProps);
      oldLayerMap.erase(matchedLayerIterator);
    } else {
      // TODO(ib@unfolded.ai): Handle exceptions
      this->addLayer(std::dynamic_pointer_cast<Layer>(layerProps->makeComponent(layerProps)));
    }
  }

  // Remove any unmatched layers
  for (const auto &unmatchedLayer : oldLayerMap) {
    // TODO(ib@unfolded.ai): Handle exceptions
    this->removeLayer(unmatchedLayer.first);
  }
}

void LayerManager::addLayer(std::shared_ptr<Layer> layer) {
  bool idExists = false;
  for (const auto &currentLayer : this->layers) {
    if (currentLayer->props()->id == layer->props()->id) {
      idExists = true;
      break;
    }
  }
  if (idExists) {
    throw std::runtime_error("Layer with id already exists");
  }

  layer->initialize(this->context);

  this->layers.push_back(layer);
}

void LayerManager::removeLayer(std::shared_ptr<Layer> layer) {
  this->layers.remove_if([layer](auto layer_) { return layer_ == layer; });
}

void LayerManager::removeLayer(const std::string &id) {
  this->layers.remove_if([id](auto layer) { return layer->props()->id == id; });
}

void LayerManager::updateLayers() {
  for (auto layer : this->layers) {
    // TODO(ib@unfolded.ai): Handle exceptions
    layer->update();
  }
}

void LayerManager::activateViewport(const std::shared_ptr<Viewport> &viewport) {
  auto oldViewport = this->context->viewport;
  auto viewportChanged = !oldViewport || oldViewport != viewport;

  if (viewportChanged) {
    this->context->viewport = viewport;

    // Update layers states
    // Let screen space layers update their state based on viewport
    for (const auto &layer : this->layers) {
      layer->setViewportChangedFlag("Viewport Activated");
      this->_updateLayer(layer);
    }
  }
}

void LayerManager::_updateLayer(const std::shared_ptr<Layer> &layer) {
  try {
    layer->update();
  } catch (const std::exception &ex) {
    probegl::ErrorLog() << "Layer update failed with: " << ex.what();
  }
}
