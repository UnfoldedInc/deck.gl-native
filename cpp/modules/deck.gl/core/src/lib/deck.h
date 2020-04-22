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

/*
import LayerManager from './layer-manager';
import ViewManager from './view-manager';
import MapView from '../views/map-view';
import EffectManager from './effect-manager';
import Effect from './effect';
import DeckRenderer from './deck-renderer';
import DeckPicker from './deck-picker';
import Tooltip from './tooltip';
import log from '../utils/log';
import {deepEqual} from '../utils/deep-equal';
import deckGlobal from './init';

import {getBrowser} from 'probe.gl/env';
import GL from '@luma.gl/constants';
import {
  AnimationLoop,
  createGLContext,
  instrumentGLContext,
  setParameters,
  Timeline,
  lumaStats
} from '@luma.gl/core';
import {Stats} from 'probe.gl';
import {EventManager} from 'mjolnir.js';

import assert from '../utils/assert';
import {EVENTS} from './constants';
*/

namespace deckgl {

class Deck : public Component {
 public:
  class Props;
  auto props() { return std::dynamic_pointer_cast<Props>(this->_props); }

  int width{100};   // Dummy value, ensure something is visible if user forgets to set window size
  int height{100};  // Dummy value, ensure something is visible if user forgets to set window size

  std::shared_ptr<lumagl::AnimationLoop> animationLoop{new lumagl::GLFWAnimationLoop()};
  std::shared_ptr<LayerContext> context{new LayerContext{this, this->animationLoop->device()}};
  std::shared_ptr<ViewManager> viewManager{new ViewManager()};
  std::shared_ptr<LayerManager> layerManager{new LayerManager{this->context}};
  std::shared_ptr<ViewState> initialViewState;
  std::shared_ptr<ViewState> viewState;

  //  private:
  std::optional<std::string> _needsRedraw;

  explicit Deck(std::shared_ptr<Deck::Props> props);
  ~Deck();

  void setProps(Deck::Props *);

  // Public API

  void run();

  // Check if a redraw is needed
  // Returns an optional string summarizing the redraw reason
  // - clearRedrawFlags (Boolean) - clear the redraw flag.
  auto needsRedraw(bool clearRedrawFlags = false) -> std::optional<std::string>;
  void redraw(bool force = false);

  auto getViews() -> std::list<std::shared_ptr<View>> { return this->viewManager->getViews(); }

  // Get a set of viewports for a given width and height
  // `getViewports`(rect);
  // `pickObject`({x, y, radius = 0, layerIds = nullptr, unproject3D})
  // `pickMultipleObjects`({x, y, radius = 0, layerIds = nullptr, unproject3D, depth = 10})
  // `pickObjects`({x, y, width = 1, height = 1, layerIds = nullptr})

 private:
  // Get the most relevant view state: props.viewState, if supplied, shadows internal viewState
  auto _getViewState() -> ViewState *;  // { return this->props->viewState || this->viewState; }

  // Get the view descriptor list
  void _getViews();
  void _drawLayers(const std::string &redrawReason);  // , renderOptions);
  void _onRendererInitialized(void *gl);
  void _onRenderFrame();      // animationProps);
  void _onViewStateChange();  // params);
};

class Deck::Props : public Component::Props {
 public:
  using super = Component::Props;

  std::string id{"deckgl"};
  int width{100};   // Dummy value, ensure something is visible if user forgets to set window size
  int height{100};  // Dummy value, ensure something is visible if user forgets to set window size

  // layer/view/controller settings
  std::list<std::shared_ptr<Layer::Props>> layers;  // PropTypes.oneOfType([PropTypes.object, PropTypes.array]),
  std::list<std::shared_ptr<View>> views;           // PropTypes.oneOfType([PropTypes.object, PropTypes.array]),
  std::shared_ptr<ViewState> viewState;             // PropTypes.object,
  std::shared_ptr<ViewState> initialViewState;

  void *_framebuffer{nullptr};  // PropTypes.object, // Experimental props
  bool _animate{false};         // PropTypes.bool // Forces a redraw every animation frame
  float pickingRadius{5};       // PropTypes.number,

  // Debug settings
  bool debug{false};
  bool drawPickingColors{false};

  // Callbacks
  std::function<void(Deck *, void *gl)> onWebGLInitialized{[](Deck *, void *gl) {}};
  std::function<void(Deck *, int width, int height)> onResize{[](Deck *, int width, int height) {}};
  std::function<auto(Deck *, ViewState *)->ViewState *> onViewStateChange{[](Deck *, ViewState *vs) { return vs; }};
  std::function<void(Deck *)> onBeforeRender{[](Deck *) {}};
  std::function<void(Deck *)> onAfterRender{[](Deck *) {}};
  std::function<void(Deck *)> onLoad{[](Deck *) {}};
  std::function<void(Deck *, const std::exception &)> onError{[](Deck *, const std::exception &) {}};

  // GL settings - these are creation parameters, can not be updated, maybe move out of props?
  // gl,
  // glOptions,
  // parameters,

  // Prop Type Machinery
  static constexpr const char *getTypeName() { return "Deck"; }
  auto getProperties() const -> const Properties * override;
  auto makeComponent(std::shared_ptr<Component::Props> props) const -> Deck * override {
    return new Deck{std::dynamic_pointer_cast<Deck::Props>(props)};
  }
};

}  // namespace deckgl

#endif  // DECK_CORE_DECK_H
