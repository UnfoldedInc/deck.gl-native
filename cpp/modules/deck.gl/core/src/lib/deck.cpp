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

#include "./deck.h"  // NOLINT(build/include)

#include <memory>

#include "../shaderlib/project/viewport-uniforms.h"
#include "luma.gl/garrow.h"

using namespace deckgl;

// Setters and getters for properties
// TODO(ib@unfolded.ai): auto generate from language-independent prop definition schema
static const std::vector<std::shared_ptr<Property>> propTypeDefs = {
    std::make_shared<PropertyT<std::list<std::shared_ptr<Layer::Props>>>>(
        "layers", [](const JSONObject* props) { return dynamic_cast<const Deck::Props*>(props)->layers; },
        [](JSONObject* props, std::list<std::shared_ptr<Layer::Props>> value) {
          return dynamic_cast<Deck::Props*>(props)->layers = value;
        }),
    std::make_shared<PropertyT<std::list<std::shared_ptr<View>>>>(
        "views", [](const JSONObject* props) { return dynamic_cast<const Deck::Props*>(props)->views; },
        [](JSONObject* props, std::list<std::shared_ptr<View>> value) {
          return dynamic_cast<Deck::Props*>(props)->views = value;
        }),
    std::make_shared<PropertyT<std::shared_ptr<ViewState>>>(
        "viewState", [](const JSONObject* props) { return dynamic_cast<const Deck::Props*>(props)->viewState; },
        [](JSONObject* props, std::shared_ptr<ViewState> value) {
          return dynamic_cast<Deck::Props*>(props)->viewState = value;
        }),
    std::make_shared<PropertyT<std::shared_ptr<ViewState>>>(
        "initialViewState",
        [](const JSONObject* props) { return dynamic_cast<const Deck::Props*>(props)->initialViewState; },
        [](JSONObject* props, std::shared_ptr<ViewState> value) {
          return dynamic_cast<Deck::Props*>(props)->initialViewState = value;
        })};

auto Deck::Props::getProperties() const -> const std::shared_ptr<Properties> {
  static auto properties = Properties::from<Deck::Props>("Deck", propTypeDefs);
  return properties;
}

Deck::Deck(std::shared_ptr<Deck::Props> props)
    : Component(props), width{props->width}, height{props->height}, _needsRedraw{"Initial render"} {
  this->animationLoop = lumagl::AnimationLoopFactory::createAnimationLoop(props->drawingOptions);
  this->context = std::make_shared<LayerContext>(this, this->animationLoop->device());
  this->layerManager = std::make_shared<LayerManager>(this->context);

  this->setProps(props);

  this->context->layerManager = this->layerManager;

  // TODO(ilija@unfolded.ai): Delegate to project shader module
  this->_viewportUniformsBuffer =
      lumagl::utils::createBuffer(this->animationLoop->device(), sizeof(ViewportUniforms), wgpu::BufferUsage::Uniform);
}

Deck::~Deck() { this->animationLoop->stop(); }

void Deck::setProps(std::shared_ptr<Deck::Props> props) {
  // ViewState tracking
  if (props->initialViewState) {
    if (!props->initialViewState->equals(this->initialViewState)) {
      // Overwrite internal view state
      this->initialViewState = props->initialViewState;
      this->viewState = props->initialViewState;
    }
  }

  if (props->viewState) {
    if (!props->viewState->equals(this->viewState)) {
      // Overwrite internal view state
      this->viewState = props->viewState;
    }
  }

  // Update layerManager
  if (!props->layers.empty()) {
    this->layerManager->setLayersFromProps(props->layers);
  }

  // Update viewManager
  this->viewManager->setWidth(props->width);
  this->viewManager->setHeight(props->height);
  this->viewManager->setViews(props->views);
  this->viewManager->setViewState(this->viewState);
  this->animationLoop->setSize({props->width, props->height});
}

void Deck::run(std::function<void(Deck*)> onAfterRender) {
  this->animationLoop->run([&](wgpu::RenderPassEncoder pass) { this->_redraw(pass, onAfterRender); });
}

void Deck::draw(wgpu::TextureView textureView, std::function<void(Deck*)> onAfterRender) {
  this->animationLoop->draw(textureView,
                            [&](wgpu::RenderPassEncoder pass) { this->_redraw(pass, onAfterRender, true); });
}

void Deck::stop() { this->animationLoop->stop(); }

auto Deck::needsRedraw(bool clearRedrawFlags) -> std::optional<std::string> {
  auto redraw = this->_needsRedraw;

  if (clearRedrawFlags) {
    this->_needsRedraw = std::nullopt;
  }

  // Query all managers to make sure we clear all flags
  auto viewManagerNeedsRedraw = this->viewManager->getNeedsRedraw(clearRedrawFlags);
  auto layerManagerNeedsRedraw = this->layerManager->needsRedraw(clearRedrawFlags);

  if (viewManagerNeedsRedraw) {
    return viewManagerNeedsRedraw;
  }
  if (layerManagerNeedsRedraw) {
    return layerManagerNeedsRedraw;
  }

  return redraw;
}

auto Deck::_getViewState() -> std::shared_ptr<ViewState> {
  if (this->props()->viewState) {
    return this->props()->viewState;
  }
  return this->viewState;
}

void Deck::_redraw(wgpu::RenderPassEncoder pass, std::function<void(Deck*)> onAfterRender, bool force) {
  // If force is falsy, check if we need to redraw
  std::optional<std::string> redrawReason = force ? "Redraw Forced" : this->needsRedraw(true);

  if (!redrawReason) {
    return;
  }

  this->_drawLayers(pass, onAfterRender, redrawReason.value());
}

void Deck::_drawLayers(wgpu::RenderPassEncoder pass, std::function<void(Deck*)> onAfterRender,
                       const std::string& redrawReason) {
  this->props()->onBeforeRender(this);

  for (auto const& viewport : this->viewManager->getViewports()) {
    // Expose the current viewport to layers for project* function
    this->layerManager->activateViewport(viewport);

    for (auto const& layer : this->layerManager->layers) {
      // TODO(ilija@unfolded.ai): Pass relevant layer properties to getUniformsFromViewport
      auto viewportUniforms = getUniformsFromViewport(viewport, this->animationLoop->devicePixelRatio());

      this->_viewportUniformsBuffer.SetSubData(0, sizeof(ViewportUniforms), &viewportUniforms);
      for (auto const& model : layer->models()) {
        // Viewport uniforms are currently bound to index 0
        model->setUniformBuffer(0, this->_viewportUniformsBuffer);
      }

      layer->draw(pass);
    }
  }

  onAfterRender(this);
  this->props()->onAfterRender(this);
}
