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

  Props *props;

  int width;   // "read-only", auto-updated from canvas
  int height;  // "read-only", auto-updated from canvas

  // Maps view descriptors to vieports, rebuilds when width/height/viewState/views change
  // std::shared_ptr<ViewManager> viewManager;
  std::shared_ptr<LayerManager> layerManager;
  // effectManager = nullptr;
  // deckRenderer = nullptr;
  // deckPicker = nullptr;

  bool _needsRedraw;
  void *_pickRequest = nullptr;
  // Pick and store the object under the pointer on `pointerdown`.
  // This object is reused for subsequent `onClick` and `onDrag*`
  // callbacks.
  void *_lastPointerDownInfo = nullptr;

  // viewState = nullptr;  // Internal view state if no callback is supplied
  // this->interactiveState = {
  //   isDragging: false // Whether the cursor is down

  explicit Deck(Deck::Props *props);
  ~Deck();

  void setProps(Deck::Props *);

  // Public API

  // Check if a redraw is needed
  // Returns `false` or a string summarizing the redraw reason
  // opts.clearRedrawFlags (Boolean) - clear the redraw flag. Default
  // `true`
  auto needsRedraw(bool clearRedrawFlags = false) -> std::string;

  void redraw(bool force = false);

  auto getViews() -> std::list<View>;  // { return this->viewManager->views; }

  /*
  // Get a set of viewports for a given width and height
  getViewports(rect) {
    return this->viewManager.getViewports(rect);
  }

  // {x, y, radius = 0, layerIds = nullptr, unproject3D}
  pickObject(opts) {
    const infos = this->_pick('pickObject', 'pickObject Time',
  opts).result; return infos.length ? infos[0] : nullptr;
  }

  // {x, y, radius = 0, layerIds = nullptr, unproject3D, depth = 10}
  pickMultipleObjects(opts) {
    opts.depth = opts.depth || 10;
    return this->_pick('pickObject', 'pickMultipleObjects Time',
  opts).result;
  }

  // {x, y, width = 1, height = 1, layerIds = nullptr}
  pickObjects(opts) {
    return this->_pick('pickObjects', 'pickObjects Time', opts);
  }
  */

  // Private Methods
  // Get the most relevant view state: props.viewState, if supplied, shadows internal viewState
  auto _getViewState() -> ViewState *;  // { return this->props->viewState || this->viewState; }

  // Get the view descriptor list
  void _getViews();

  /*
  auto _pick(method, statKey, opts);

  // canvas, either string, canvas or `nullptr`
  auto _createCanvas(props);

  // Updates canvas width and/or height, if provided as props
  auto _setCanvasSize(props);

  // If canvas size has changed, updates
  auto _updateCanvasSize();

  // If canvas size has changed, reads out the new size and returns true
  auto _checkForCanvasSizeChange();

  void _createAnimationLoop(props);

  // The `pointermove` event may fire multiple times in between two animation frames,
  // it's a waste of time to run picking without rerender. Instead we save the last pick
  // request and only do it once on the next animation frame.
  void _onPointerMove(event);

  // Actually run picking
  void _pickAndCallback();

  void _updateCursor();

  void _setGLContext(void *gl);
  */

  void _drawLayers(const std::string &redrawReason);  // , renderOptions);
  // Callbacks

  void _onRendererInitialized(void *gl);

  void _onRenderFrame();  // animationProps);

  // Callbacks

  void _onViewStateChange();  // params);

  /*
  void _onInteractiveStateChange(isDragging = false);

  void _onEvent(event);

  void _onPointerDown(event);

  void _getFrameStats();

  void _getMetrics();
  */
};

class Deck::Props : public Component::Props {
 public:
  using super = Component::Props;
  static constexpr const char *getTypeName() { return "Deck"; }

  std::string id;  // PropTypes.string,
  int width;       // PropTypes.oneOfType([PropTypes.number, PropTypes.string]),
  int height;      // PropTypes.oneOfType([PropTypes.number, PropTypes.string]),

  // layer/view/controller settings
  std::list<std::shared_ptr<Layer::Props>> layers;  // PropTypes.oneOfType([PropTypes.object, PropTypes.array]),
  std::list<std::shared_ptr<View::Props>> views;    // PropTypes.oneOfType([PropTypes.object, PropTypes.array]),
  std::shared_ptr<ViewState> viewState;             // PropTypes.object,
  std::shared_ptr<ViewState> initialViewState;

  // effects, // PropTypes.arrayOf(PropTypes.instanceOf(Effect)),
  // controller, // PropTypes.oneOfType([PropTypes.func, PropTypes.bool, PropTypes.object]),
  // std::string touchAction, // PropTypes.string,

  // GL settings
  // gl, // PropTypes.object,
  // glOptions, // PropTypes.object,
  // parameters, // PropTypes.object,
  void *_framebuffer;    // PropTypes.object, // Experimental props
  bool _animate;         // PropTypes.bool // Forces a redraw every animation frame
  float pickingRadius;   // PropTypes.number,
  bool useDevicePixels;  // PropTypes.oneOfType([PropTypes.bool, PropTypes.number]),

  // Debug settings
  bool debug;              // PropTypes.bool;
  bool drawPickingColors;  // PropTypes.bool;

  // Callbacks
  // std::function<> layerFilter, // PropTypes.func,
  std::function<void(Deck *, void *gl)> onWebGLInitialized;
  std::function<void(Deck *, int width, int height)> onResize;
  std::function<auto(Deck *, const ViewState *)->ViewState *> onViewStateChange;
  std::function<void(Deck *)> onBeforeRender;
  std::function<void(Deck *)> onAfterRender;
  std::function<void(Deck *)> onLoad;
  std::function<void(Deck *, const std::exception &)> onError;

  Props();
  auto getProperties() const -> const Properties *;
};

/*
function noop() {}

const getCursor = ({isDragging}) => (isDragging ? 'grabbing' : 'grab');

function getPropTypes(PropTypes) {
  // Note: Arrays (layers, views, ) can contain falsy values
  return {
    id: PropTypes.string,
    width: PropTypes.oneOfType([PropTypes.number, PropTypes.string]),
    height: PropTypes.oneOfType([PropTypes.number, PropTypes.string]),

    // layer/view/controller settings
    layers: PropTypes.oneOfType([PropTypes.object, PropTypes.array]),
    layerFilter: PropTypes.func,
    views: PropTypes.oneOfType([PropTypes.object, PropTypes.array]),
    viewState: PropTypes.object,
    effects: PropTypes.arrayOf(PropTypes.instanceOf(Effect)),
    controller: PropTypes.oneOfType([PropTypes.func, PropTypes.bool, PropTypes.object]),

    // GL settings
    gl: PropTypes.object,
    glOptions: PropTypes.object,
    parameters: PropTypes.object,
    pickingRadius: PropTypes.number,
    useDevicePixels: PropTypes.oneOfType([PropTypes.bool, PropTypes.number]), touchAction: PropTypes.string,

    // Callbacks
    onWebGLInitialized: PropTypes.func,
    onResize: PropTypes.func,
    onViewStateChange: PropTypes.func,
    onBeforeRender: PropTypes.func,
    onAfterRender: PropTypes.func,
    onLoad: PropTypes.func,
    onError: PropTypes.func,

    // Debug settings
    debug: PropTypes.bool,
    drawPickingColors: PropTypes.bool,

    // Experimental props
    _framebuffer: PropTypes.object,
    // Forces a redraw every animation frame
    _animate: PropTypes.bool
  };
}
*/

}  // namespace deckgl

#endif  // DECK_CORE_DECK_H
