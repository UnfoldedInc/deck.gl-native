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

#include "./view-manager.h"  // NOLINT(build/include)

#include <array>
#include <iostream>

#undef PI
#include "range/v3/all.hpp"

using namespace deckgl;

ViewManager::ViewManager() {}

ViewManager::~ViewManager() {}

// Check if a redraw is needed
auto ViewManager::getNeedsRedraw(bool clearRedrawFlags) -> std::optional<std::string> {
  auto redraw = this->_needsRedraw;
  if (clearRedrawFlags) {
    this->_needsRedraw = std::nullopt;
  }
  return redraw;
}

// Layers will be updated deeply (in next animation frame)
// Potentially regenerating attributes and sub layers
void ViewManager::setNeedsUpdate(const std::string &reason) {
  this->_needsUpdate = this->_needsUpdate.value_or(reason);
  this->_needsRedraw = this->_needsRedraw.value_or(reason);
}

// Checks each viewport for transition updates
// void ViewManager::updateViewStates() {
//   for (const viewId in this->controllers) {
//     const controller = this->controllers[viewId];
//     if (controller) {
//       controller.updateTransition();
//     }
//   }
// }

/** Get a set of viewports for a given width and height
 * TODO - Intention is for deck.gl to autodeduce width and height and drop
 * the need for props
 * @param rect (object, optional) - filter the viewports
 *   + not provided - return all viewports
 *   + {x, y} - only return viewports that contain this pixel
 *   + {x, y, width, height} - only return viewports that overlap with this
 * rectangle
 */
auto ViewManager::getViewports() -> std::list<std::shared_ptr<Viewport>> {  // {rect) {
  // if (rect) {
  //   return this->_viewports.filter(viewport = > viewport.containsPixel(rect));
  // }
  return this->_viewports;
}

auto ViewManager::getViews() -> std::list<std::shared_ptr<View>> {
  // const viewMap = {};
  // this->views.forEach(view = > { viewMap[view.id] = view; });
  // return viewMap;
  return this->views;
}

// Resolves a viewId string to a View, if already a View returns it.
auto ViewManager::getView(const std::string &viewId) -> std::shared_ptr<View> {
  // TODO(ib): implement a lookup
  return this->views.front();
}

// Returns the viewState for a specific viewId. Matches the viewState by
// 1. view.viewStateId
// 2. view.id
// 3. root viewState
// then applies the view's filter if any
// ViewManager::getViewState(viewId) {
//   const view = this->getView(viewId);
//   // Backward compatibility: view state for single view
//   const viewState = (view && this->viewState[view.getViewStateId()]) || this->viewState;
//   return view ? view.filterViewState(viewState) : viewState;
// }

auto ViewManager::getViewport(const std::string &viewId) -> std::shared_ptr<Viewport> {
  return nullptr;  // return this->_viewportMap[viewId];
}

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
auto ViewManager::unproject(const mathgl::Vector3<double> xyz, bool topLeft) -> std::optional<mathgl::Vector3<double>> {
  auto viewports = this->getViewports();
  for (auto i = viewports.rbegin(); i != viewports.rend(); i++) {
    auto viewport = *i;
    if (viewport->containsPixel(xyz.x, xyz.y)) {
      auto p = xyz;
      p.x -= viewport->x;
      p.y -= viewport->y;
      return viewport->unproject(p, topLeft);
    }
  }
  return std::nullopt;
}

auto ViewManager::unproject(const mathgl::Vector2<double> xy, bool topLeft) -> std::optional<mathgl::Vector2<double>> {
  auto viewports = this->getViewports();
  for (auto i = viewports.rbegin(); i != viewports.rend(); i++) {
    auto viewport = *i;
    if (viewport->containsPixel(xy.x, xy.y)) {
      auto p = xy;
      p.x -= viewport->x;
      p.y -= viewport->y;
      return viewport->unproject(p, topLeft);
    }
  }
  return std::nullopt;
}

//
// MODIFIERS
//

void ViewManager::setSize(int width, int height) {
  this->setWidth(width);
  this->setHeight(height);
}

void ViewManager::setWidth(int width) {
  if (width < 0) {
    width = 0;
  }

  if (width != this->width) {
    this->width = width;
    this->setNeedsUpdate("Window width changed");
  }
}

void ViewManager::setHeight(int height) {
  if (height < 0) {
    height = 0;
  }

  // Only trigger an update if height actually changed
  if (height != this->height) {
    this->height = height;
    this->setNeedsUpdate("Window height changed");
  }
}

// Update the view descriptor list and set change flag if needed
// Does not actually rebuild the `Viewport`s until `getViewports` is called
void ViewManager::setViews(const std::list<std::shared_ptr<View>> &views) {
  // TODO(ib): Only update if views actually changed
  auto viewsChanged = true;  // this->_diffViews(views, this->views);
  if (viewsChanged) {
    this->views = views;
    this->setNeedsUpdate("Views changed");
  }
}

void ViewManager::setViewState(std::shared_ptr<ViewState> viewState) {
  auto viewStateChanged = !viewState->equals(this->viewState);

  if (viewStateChanged) {
    this->viewState = viewState;
    this->setNeedsUpdate("viewState changed");
  }
}

// void ViewManager::setViewStates(const std::list<std::shared_ptr<ViewState>> &viewStates) {
//   if (viewState) {
//     auto viewStateChanged = !viewState->equals(this->viewState);

//     if (viewStateChanged) {
//       this->setNeedsUpdate("viewState changed");
//     }

//     this->viewState = viewStates;
//   } else {
//     // log.warn('missing `viewState` or `initialViewState`')();
//   }
// }

//
// PRIVATE METHODS
//

void ViewManager::_update() {
  // Important: avoid invoking _update() inside itself
  // Nested updates result in unexpected side effects inside
  // _rebuildViewports() when using auto control in pure-js
  // if (!this->_isUpdating) {
  //   this->_update();
  // }

  this->_isUpdating = true;

  // Only rebuild viewports if the update flag is set
  if (this->_needsUpdate) {
    this->_needsUpdate = std::nullopt;
    this->_rebuildViewports();
  }

  // If viewport transition(s) are triggered during viewports update,
  // controller(s) will immediately call `onViewStateChange` which calls
  // `viewManager.setProps` again.
  if (this->_needsUpdate) {
    this->_needsUpdate = std::nullopt;
    this->_rebuildViewports();
  }

  this->_isUpdating = false;
}

// Rebuilds viewports from descriptors towards a certain window size
void ViewManager::_rebuildViewports() {
  throw std::logic_error("rebuild viewports not implemented");
  // const {width, height, views} = this;

  // this->_viewports = [];
  // this->controllers = {};

  // for (let i = views.length; i--;) {
  //   const view = views[i];
  //   const viewState = this->getViewState(view);
  //   const viewport = view.makeViewport({width, height, viewState});

  //   this->_viewports.unshift(viewport);
  // }

  // this->_buildViewportMap();
}

// ViewManager::_buildViewportMap() {
//   // Build a view id to view index
//   this->_viewportMap = {};
//   this->_viewports.forEach(viewport = > {
//     if (viewport.id) {
//       // TODO - issue warning if multiple viewports use same id
//       this->_viewportMap[viewport.id] = this->_viewportMap[viewport.id] || viewport;
//     }
//   });
// }

// Check if viewport array has changed, returns true if any change
// Note that descriptors can be the same
// ViewManager::_diffViews(const std::list<std::shared_ptr<View>> &views, const std::list<std::shared_ptr<View>>
// &views)
// {
//   if (newViews.length != = oldViews.length) {
//     return true;
//   }

//   return newViews.some((_, i) = > !newViews[i].equals(oldViews[i]));
// }
