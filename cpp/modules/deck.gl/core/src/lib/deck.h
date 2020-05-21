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

namespace deckgl {

class Deck : public Component {
 public:
  class Props;
  auto props() { return std::dynamic_pointer_cast<Props>(this->_props); }
  void setProps(std::shared_ptr<Deck::Props> props);

  struct RenderingOptions;
  explicit Deck(std::shared_ptr<Deck::Props> props = std::make_shared<Deck::Props>());
  ~Deck();

  int width{100};   // Dummy value, ensure something is visible if user forgets to set window size
  int height{100};  // Dummy value, ensure something is visible if user forgets to set window size

  void run(std::function<void(Deck*)> onAfterRender = [](Deck*) {});
  void draw(
      wgpu::TextureView textureView, std::function<void(Deck*)> onAfterRender = [](Deck*) {});
  void stop();

  // Check if a redraw is needed
  // Returns an optional string summarizing the redraw reason
  // - clearRedrawFlags (Boolean) - clear the redraw flag.
  auto needsRedraw(bool clearRedrawFlags = false) -> std::optional<std::string>;
  void redraw(bool force = false);

  auto getViews() -> std::list<std::shared_ptr<View>> { return this->viewManager->getViews(); }

  std::shared_ptr<lumagl::AnimationLoop> animationLoop;
  std::shared_ptr<ViewManager> viewManager{new ViewManager()};
  std::shared_ptr<LayerContext> context;
  std::shared_ptr<LayerManager> layerManager;
  std::shared_ptr<ViewState> initialViewState;
  std::shared_ptr<ViewState> viewState;

 private:
  // Get the most relevant view state: props.viewState, if supplied, shadows internal viewState
  auto _getViewState() -> ViewState*;  // { return this->props->viewState || this->viewState; }

  auto _createAnimationLoop(const std::shared_ptr<Deck::Props>& props) -> std::shared_ptr<lumagl::AnimationLoop>;
  // Get the view descriptor list
  void _getViews();
  void _draw(wgpu::RenderPassEncoder pass, std::function<void(Deck*)> onAfterRender);
  void _drawLayers(const std::string& redrawReason);
  void _onRendererInitialized(void* gl);
  void _onRenderFrame();
  void _onViewStateChange();

  std::optional<std::string> _needsRedraw;
  wgpu::Buffer _viewportUniformsBuffer;
};

class Deck::Props : public Component::Props {
 public:
  using super = Component::Props;

  std::string id{"deckgl"};
  int width{100};   // Dummy value, ensure something is visible if user forgets to set window size
  int height{100};  // Dummy value, ensure something is visible if user forgets to set window size

  std::shared_ptr<lumagl::AnimationLoop::Options> drawingOptions;

  // layer/view/controller settings
  std::list<std::shared_ptr<Layer::Props>> layers;
  std::list<std::shared_ptr<View>> views;
  std::shared_ptr<ViewState> initialViewState;
  std::shared_ptr<ViewState> viewState;

  // Debug settings
  bool debug{false};

  // Callbacks
  std::function<void(Deck*)> onBeforeRender{[](Deck*) {}};
  std::function<void(Deck*)> onAfterRender{[](Deck*) {}};

  //  std::function<void(Deck*, void* gl)> onWebGLInitialized{[](Deck*, void* gl) {}};
  //  std::function<void(Deck*, int width, int height)> onResize{[](Deck*, int width, int height) {}};
  //  std::function<auto(Deck*, ViewState*) -> ViewState*> onViewStateChange{[](Deck*, ViewState* vs) { return vs; }};
  //  std::function<void(Deck*)> onLoad{[](Deck*) {}};
  //  std::function<void(Deck*, const std::exception &)> onError{[](Deck*, const std::exception &) {}};

  // Prop Type Machinery
  static constexpr const char* getTypeName() { return "Deck"; }
  auto getProperties() const -> const Properties* override;
  auto makeComponent(std::shared_ptr<Component::Props> props) const -> Deck* override {
    return new Deck{std::dynamic_pointer_cast<Deck::Props>(props)};
  }
};

}  // namespace deckgl

#endif  // DECK_CORE_DECK_H
