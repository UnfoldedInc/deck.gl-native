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

#ifndef DECKGL_CORE_LAYER_MANAGER_H
#define DECKGL_CORE_LAYER_MANAGER_H

#include "./layer.h"

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

namespace deckgl {

class Deck;
class LayerManager;

class DeckContext {
 public:
  Deck *deck;
  LayerManager *layerManager;
  // gl,

  // // General resources
  // stats: null, // for tracking lifecycle performance
  // // GL Resources
  // shaderCache: null,
  // pickingFBO: null, // Screen-size framebuffer that layers can reuse
  // mousePosition: null,
  // userData: {} // Place for any custom app `context`

  DeckContext(Deck *deck_, LayerManager *layerManager_)
      : deck{deck_},
        layerManager{layerManager_}  // , gl{nullptr}
                                     // , stats
  {}
};

class LayerManagerProps {
 public:
};

class LayerManager {
 public:
  DeckContext context;
  std::list<Layer *> layers;
  std::list<Layer *> lastRenderedLayers;

  std::string _needsRedraw;
  std::string _needsUpdate;
  bool _debug;

  LayerManager(Deck *deck);

  // Method to call when the layer manager is not needed anymore.
  ~LayerManager();

  // Check if a redraw is needed
  auto needsRedraw(bool clearRedrawFlags = false) -> std::string;

  // Check if a deep update of all layers is needed
  auto needsUpdate() -> std::string;  // { return this->_needsUpdate; }

  // Layers will be redrawn (in next animation frame)
  void setNeedsRedraw(const std::string &reason);  // { this->_needsRedraw = this->_needsRedraw || reason; }

  // Layers will be updated deeply (in next animation frame)
  // Potentially regenerating attributes and sub layers
  void setNeedsUpdate(const std::string &reason);  // { this->_needsUpdate = this->_needsUpdate || reason; }

  // Gets an (optionally) filtered list of layers
  auto getLayers(const std::list<std::string> &layerIds = std::list<std::string>{}) -> std::list<Layer *>;

  // Set props needed for layer rendering and picking.
  /*
  void setProps(props) {
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
  void setLayers(const std::list<Layer *> newLayers, bool forceUpdate = false);

  // Update layers from last cycle if `setNeedsUpdate()` has been called
  void updateLayers();

  //
  // PRIVATE METHODS
  //

  // Make a viewport "current" in layer context, updating viewportChanged
  // flags
  /*
  void activateViewport(viewport) {

  void _handleError(stage, error, layer) {
  */

  // Match all layers, checking for caught errors
  // To avoid having an exception in one layer disrupt other layers
  // TODO - mark layers with exceptions as bad and remove from rendering
  // cycle?
  void _updateLayers(const std::list<Layer *> &oldLayers, const std::list<Layer *> &newLayers);

  /*
  // Note: adds generated layers to `generatedLayers` array parameter
  void _updateSublayersRecursively(newLayers, oldLayerMap, generatedLayers) {
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
  void _finalizeOldLayers(const std::map<std::string, Layer *> &oldLayerMap);

  // EXCEPTION SAFE LAYER ACCESS

  // Initializes a single layer, calling layer methods
  void _initializeLayer(Layer *);

  void _transferLayerState(Layer *oldLayer, Layer *newLayer);

  // Updates a single layer, cleaning all flags
  void _updateLayer(Layer *);

  // Finalizes a single layer
  void _finalizeLayer(Layer *);
};

}  // namespace deckgl

#endif  // DECKGL_CORE_LAYER_MANAGER_H
