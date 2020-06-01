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

#include <list>
#include <map>
#include <memory>
#include <optional>
#include <string>

#include "./layer-context.h"
#include "./layer.h"

namespace deckgl {

class LayerManager {
 public:
  std::shared_ptr<LayerContext> context;

  std::list<std::shared_ptr<Layer>> layers;

  explicit LayerManager(std::shared_ptr<LayerContext> context);
  virtual ~LayerManager();

  /// \brief Check if a redraw is needed.
  auto needsRedraw(bool clearRedrawFlags = false) -> std::optional<std::string>;
  /// \brief Check if a deep update of layers is needed.
  auto needsUpdate() -> std::optional<std::string> { return this->_needsUpdate; };
  /// \brief Ensures that layers will be redrawn (during next render).
  void setNeedsRedraw(const std::string& reason);
  /// \brief Ensure that layers will be updated deeply (during next render), including sublayer generation.
  void setNeedsUpdate(const std::string& reason);

  void setDebug(bool debug) { this->_debug = debug; }

  /* Layer API */

  /// \brief For JSON: Supply a new layer prop list.
  void setLayersFromProps(const std::list<std::shared_ptr<Layer::Props>>& newLayers);

  void addLayer(std::shared_ptr<Layer>);
  void removeLayer(std::shared_ptr<Layer>);
  void removeLayer(const std::string& id);

  /// \brief Update layers from last cycle if `setNeedsUpdate()` has been called.
  void updateLayers();

  /// \brief Make a viewport "current" in layer context, updating viewportChanged flags.
  void activateViewport(const std::shared_ptr<Viewport>& viewport);

 private:
  /// \brief Updates a single layer, cleaning all flags.
  void _updateLayer(const std::shared_ptr<Layer>& layer);

  std::optional<std::string> _needsRedraw;
  std::optional<std::string> _needsUpdate;
  bool _debug;
};

}  // namespace deckgl

#endif  // DECKGL_CORE_LAYER_MANAGER_H
