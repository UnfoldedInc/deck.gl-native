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

#include "./layer-context.h"
#include "./layer.h"

/*
import {LIFECYCLE} from '../lifecycle/constants';
import Viewport from '../viewports/viewport';
import {createProgramManager} from '../shaderlib';
*/

namespace deckgl {

class LayerManager {
 public:
  LayerContext *context;

  std::list<Layer *> layers;
  std::list<Layer *> lastRenderedLayers;

  std::optional<std::string> _needsRedraw;
  std::optional<std::string> _needsUpdate;
  bool _debug;

  LayerManager(LayerContext *context);
  virtual ~LayerManager();

  // Check if a redraw is needed
  auto needsRedraw(bool clearRedrawFlags = false) -> std::optional<std::string>;
  // Check if a deep update of layers is needed
  auto needsUpdate() -> std::optional<std::string>;
  // Ensures that layers will be redrawn (during next render)
  void setNeedsRedraw(const std::string &reason);
  // Ensiure that layers will be updated deeply (during next render), including sublayer generation
  void setNeedsUpdate(const std::string &reason);

  // Props

  void setDebug(bool debug);
  void setUserData(void *userData);
  void setOnError();

  // Layer API

  void addLayer(std::shared_ptr<Layer>);
  // Gets an (optionally) filtered list of layers
  auto getLayers(const std::list<std::string> &layerIds = std::list<std::string>{})
      -> std::list<std::shared_ptr<Layer>>;
  auto findLayerById(const std::string &id) -> std::shared_ptr<Layer>;
  void removeAllLayers();
  void removeLayer(std::shared_ptr<Layer>);
  void removeLayerById(const std::string &id);

  // For JSON: Supply a new layer prop list
  void setLayerProps(const std::list<Layer::Props> newLayers);

  // Update layers from last cycle if `setNeedsUpdate()` has been called
  void updateLayers();

  //
  // PRIVATE METHODS
  //

  // Make a viewport "current" in layer context, updating viewportChanged flags
  // void activateViewport(viewport);

  // void _handleError(stage, error, layer);

  // Match all layers, checking for caught errors
  // To avoid having an exception in one layer disrupt other layers
  // TODO - mark layers with exceptions as bad and remove from rendering cycle?
  void _updateLayers(const std::list<Layer> &oldLayers, const std::list<Layer::Props> &newLayers);

  // Finalize any old layers that were not matched
  void _finalizeOldLayers(const std::map<std::string, Layer *> &oldLayerMap);

  // EXCEPTION SAFE LAYER ACCESS

  // Initializes a single layer, calling layer methods
  void _initializeLayer(Layer *);

  // void _transferLayerState(Layer *oldLayer, Layer *newLayer);

  // Updates a single layer, cleaning all flags
  void _updateLayer(Layer *);

  // Finalizes a single layer
  void _finalizeLayer(Layer *);
};

}  // namespace deckgl

#endif  // DECKGL_CORE_LAYER_MANAGER_H
