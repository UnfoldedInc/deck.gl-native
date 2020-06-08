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

#ifndef DECKGL_CORE_DECK_H
#define DECKGL_CORE_DECK_H

#include <list>
#include <memory>
#include <optional>
#include <string>

#include "./layer-manager.h"
#include "./view-manager.h"
#include "deck.gl/json.h"
#include "luma.gl/core.h"
#include "probe.gl/core.h"

namespace deckgl {

class Deck : public Component {
 public:
  class Props;
  struct RenderingOptions;

#pragma mark - Exception-free API

  static auto make(probegl::Error& error, std::shared_ptr<Deck::Props> props = std::make_shared<Deck::Props>()) noexcept
      -> std::shared_ptr<Deck>;
  void setProps(std::shared_ptr<Deck::Props> props, probegl::Error& error) noexcept;

  void run(
      probegl::Error& error, std::function<void(Deck*)> onAfterRender = [](Deck*) {}) noexcept;
  void draw(
      wgpu::TextureView textureView, probegl::Error& error,
      std::function<void(Deck*)> onAfterRender = [](Deck*) {}) noexcept;

#pragma mark -

  explicit Deck(std::shared_ptr<Deck::Props> props = std::make_shared<Deck::Props>());
  ~Deck();

  auto props() { return std::dynamic_pointer_cast<Props>(this->_props); }
  void setProps(std::shared_ptr<Deck::Props> props);

  void run(std::function<void(Deck*)> onAfterRender = [](Deck*) {});
  void draw(
      wgpu::TextureView textureView, std::function<void(Deck*)> onAfterRender = [](Deck*) {});
  void stop();

  /// \brief Check if a redraw is needed.
  /// \param clearRedrawFlags Whether needsRedraw flag should be cleared or not.
  /// \returns Returns an optional string summarizing the redraw reason.
  auto needsRedraw(bool clearRedrawFlags = false) -> std::optional<std::string>;

  auto getViews() -> std::list<std::shared_ptr<View>> { return this->viewManager->getViews(); }

  auto size() -> lumagl::Size { return this->_size; };

  std::shared_ptr<lumagl::AnimationLoop> animationLoop;
  std::shared_ptr<ViewManager> viewManager{new ViewManager()};
  std::shared_ptr<LayerContext> context;
  std::shared_ptr<LayerManager> layerManager;
  std::shared_ptr<ViewState> initialViewState;
  std::shared_ptr<ViewState> viewState;

 private:
  /// \brief Get the most relevant view state: props.viewState, if supplied, shadows internal viewState.
  auto _getViewState() -> std::shared_ptr<ViewState>;

  void _setSize(const lumagl::Size& size);

  void _redraw(wgpu::RenderPassEncoder pass, std::function<void(Deck*)> onAfterRender, bool force = false);
  void _drawLayers(wgpu::RenderPassEncoder pass, std::function<void(Deck*)> onAfterRender,
                   const std::string& redrawReason);

  std::optional<std::string> _needsRedraw;
  wgpu::Buffer _viewportUniformsBuffer;
  lumagl::Size _size;
};

// Instead of maintaining another structure with options, we reuse the relevant struct from lumagl
using DrawingOptions = lumagl::AnimationLoop::Options;
class Deck::Props : public Component::Props {
 public:
  using super = Component::Props;

  int width{100};   // Dummy value, ensure something is visible if user forgets to set window size
  int height{100};  // Dummy value, ensure something is visible if user forgets to set window size

  std::shared_ptr<DrawingOptions> drawingOptions;

  // Layer/View/Controller settings
  std::list<std::shared_ptr<Layer::Props>> layers;
  std::list<std::shared_ptr<View>> views;
  std::shared_ptr<ViewState> initialViewState;
  std::shared_ptr<ViewState> viewState;

  // Debug settings
  bool debug{false};

  // Callbacks
  std::function<void(Deck*)> onBeforeRender{[](Deck*) {}};
  std::function<void(Deck*)> onAfterRender{[](Deck*) {}};

  // Prop Type Machinery
  static constexpr const char* getTypeName() { return "Deck"; }
  auto getProperties() const -> const std::shared_ptr<Properties> override;
  auto makeComponent(std::shared_ptr<Component::Props> props) const -> std::shared_ptr<Component> override {
    return std::make_shared<Deck>(std::dynamic_pointer_cast<Deck::Props>(props));
  }
};

}  // namespace deckgl

#endif  // DECK_CORE_DECK_H
