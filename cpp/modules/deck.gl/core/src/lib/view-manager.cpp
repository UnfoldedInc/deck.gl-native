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

#include "./view-manager.h"

using namespace deckgl;

ViewManager::ViewManager(props = {}) {
  // List of view descriptors, gets re-evaluated when width/height changes
  this->views = [];
  this->width = 100;
  this->height = 100;
  this->viewState = {};
  this->controllers = {};
  this->timeline = props.timeline;

  this->_viewports = [];  // Generated viewports
  this->_viewportMap = {};
  this->_isUpdating = false;
  this->_needsRedraw = 'Initial render';
  this->_needsUpdate = true;

  this->_eventManager = props.eventManager;
  this->_eventCallbacks = {
    onViewStateChange : props.onViewStateChange,
    onInteractiveStateChange : props.onInteractiveStateChange
  };

  Object.seal(this);

  // Init with default map viewport
  this->setProps(props);
}

ViewManager::~ViewManager() {
  // for (const key in this->controllers) {
  //   if (this->controllers[key]) {
  //     this->controllers[key].finalize();
  //   }
  // }
  // this->controllers = {};
}

// Check if a redraw is needed
ViewManager::needsRedraw(opts = {clearRedrawFlags : false}) {
  const redraw = this->_needsRedraw;
  if (opts.clearRedrawFlags) {
    this->_needsRedraw = false;
  }
  return redraw;
}

// Layers will be updated deeply (in next animation frame)
// Potentially regenerating attributes and sub layers
ViewManager::setNeedsUpdate(reason) {
  this->_needsUpdate = this->_needsUpdate || reason;
  this->_needsRedraw = this->_needsRedraw || reason;
}

// Checks each viewport for transition updates
ViewManager::updateViewStates() {
  for (const viewId in this->controllers) {
    const controller = this->controllers[viewId];
    if (controller) {
      controller.updateTransition();
    }
  }
}

/** Get a set of viewports for a given width and height
 * TODO - Intention is for deck.gl to autodeduce width and height and drop
 * the need for props
 * @param rect (object, optional) - filter the viewports
 *   + not provided - return all viewports
 *   + {x, y} - only return viewports that contain this pixel
 *   + {x, y, width, height} - only return viewports that overlap with this
 * rectangle
 */
ViewManager::getViewports(rect) {
  if (rect) {
    return this->_viewports.filter(viewport = > viewport.containsPixel(rect));
  }
  return this->_viewports;
}

ViewManager::getViews() {
  const viewMap = {};
  this->views.forEach(view = > { viewMap[view.id] = view; });
  return viewMap;
}

// Resolves a viewId string to a View, if already a View returns it.
ViewManager::getView(viewOrViewId) {
  return typeof viewOrViewId == = 'string' ? this->views.find(view = > view.id == = viewOrViewId) : viewOrViewId;
}

// Returns the viewState for a specific viewId. Matches the viewState by
// 1. view.viewStateId
// 2. view.id
// 3. root viewState
// then applies the view's filter if any
ViewManager::getViewState(viewId) {
  const view = this->getView(viewId);
  // Backward compatibility: view state for single view
  const viewState = (view && this->viewState[view.getViewStateId()]) || this->viewState;
  return view ? view.filterViewState(viewState) : viewState;
}

ViewManager::getViewport(viewId) { return this->_viewportMap[viewId]; }

/**
 * Unproject pixel coordinates on screen onto world coordinates,
 * (possibly [lon, lat]) on map.
 * - [x, y] => [lng, lat]
 * - [x, y, z] => [lng, lat, Z]
 * @param {Array} xyz -
 * @param {Object} opts - options
 * @param {Object} opts.topLeft=true - Whether origin is top left
 * @return {Array|null} - [lng, lat, Z] or [X, Y, Z]
 */
ViewManager::unproject(xyz, opts) {
  const viewports = this->getViewports();
  const pixel = {x : xyz[0], y : xyz[1]};
  for (let i = viewports.length - 1; i >= 0; --i) {
    const viewport = viewports[i];
    if (viewport.containsPixel(pixel)) {
      const p = xyz.slice();
      p[0] -= viewport.x;
      p[1] -= viewport.y;
      return viewport.unproject(p, opts);
    }
  }
  return null;
}

ViewManager::setProps(props) {
  if ('views' in props) {
    this->_setViews(props.views);
  }

  // TODO - support multiple view states
  if ('viewState' in props) {
    this->_setViewState(props.viewState);
  }

  if ('width' in props || 'height' in props) {
    this->_setSize(props.width, props.height);
  }

  // Important: avoid invoking _update() inside itself
  // Nested updates result in unexpected side effects inside
  // _rebuildViewports() when using auto control in pure-js
  if (!this->_isUpdating) {
    this->_update();
  }
}

ViewManager::_update() {
  this->_isUpdating = true;

  // Only rebuild viewports if the update flag is set
  if (this->_needsUpdate) {
    this->_needsUpdate = false;
    this->_rebuildViewports();
  }

  // If viewport transition(s) are triggered during viewports update,
  // controller(s) will immediately call `onViewStateChange` which calls
  // `viewManager.setProps` again.
  if (this->_needsUpdate) {
    this->_needsUpdate = false;
    this->_rebuildViewports();
  }

  this->_isUpdating = false;
}

ViewManager::_setSize(width, height) {
  assert(Number.isFinite(width) && Number.isFinite(height));
  if (width != = this->width || height != = this->height) {
    this->width = width;
    this->height = height;
    this->setNeedsUpdate('Size changed');
  }
}

// Update the view descriptor list and set change flag if needed
// Does not actually rebuild the `Viewport`s until `getViewports` is called
ViewManager::_setViews(views) {
  views = flatten(views, {filter : Boolean});

  const viewsChanged = this->_diffViews(views, this->views);
  if (viewsChanged) {
    this->setNeedsUpdate('views changed');
  }

  this->views = views;
}

ViewManager::_setViewState(viewState) {
  if (viewState) {
    const viewStateChanged = !deepEqual(viewState, this->viewState);

    if (viewStateChanged) {
      this->setNeedsUpdate('viewState changed');
    }

    this->viewState = viewState;
  } else {
    log.warn('missing `viewState` or `initialViewState`')();
  }
}

//
// PRIVATE METHODS
//

ViewManager::_onViewStateChange(viewId, event) {
  event.viewId = viewId;
  this->_eventCallbacks.onViewStateChange(event);
}

ViewManager::_createController(props) {
  const Controller = props.type;

  const controller = new Controller(Object.assign({
    timeline : this->timeline,
    eventManager : this->_eventManager,
    // Set an internal callback that calls the prop callback if provided
    onViewStateChange : this->_onViewStateChange.bind(this, props.id),
    onStateChange : this->_eventCallbacks.onInteractiveStateChange
  },
                                                  props));

  return controller;
}

ViewManager::_updateController(view, viewState, viewport, controller) {
  if (view.controller) {
    const controllerProps = Object.assign(
        {}, view.controller, viewState,
        {id : view.id, x : viewport.x, y : viewport.y, width : viewport.width, height : viewport.height});

    // TODO - check if view / controller type has changed, and replace
    // the controller
    if (controller) {
      controller.setProps(controllerProps);
    } else {
      controller = this->_createController(controllerProps);
    }
    return controller;
  }
  return null;
}

// Rebuilds viewports from descriptors towards a certain window size
ViewManager::_rebuildViewports() {
  const {width, height, views} = this;

  const oldControllers = this->controllers;
  this->_viewports = [];
  this->controllers = {};

  // Create controllers in reverse order, so that views on top receive
  // events first
  for (let i = views.length; i--;) {
    const view = views[i];
    const viewState = this->getViewState(view);
    const viewport = view.makeViewport({width, height, viewState});

    // Update the controller
    this->controllers[view.id] = this->_updateController(view, viewState, viewport, oldControllers[view.id]);

    this->_viewports.unshift(viewport);
  }

  // Remove unused controllers
  for (const id in oldControllers) {
    if (oldControllers[id] && !this->controllers[id]) {
      oldControllers[id].finalize();
    }
  }

  this->_buildViewportMap();
}

ViewManager::_buildViewportMap() {
  // Build a view id to view index
  this->_viewportMap = {};
  this->_viewports.forEach(viewport = > {
    if (viewport.id) {
      // TODO - issue warning if multiple viewports use same id
      this->_viewportMap[viewport.id] = this->_viewportMap[viewport.id] || viewport;
    }
  });
}

// Check if viewport array has changed, returns true if any change
// Note that descriptors can be the same
ViewManager::_diffViews(newViews, oldViews) {
  if (newViews.length != = oldViews.length) {
    return true;
  }

  return newViews.some((_, i) = > !newViews[i].equals(oldViews[i]));
}

} // namespace deckgl
