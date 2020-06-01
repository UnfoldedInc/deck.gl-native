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

auto ViewManager::getNeedsRedraw(bool clearRedrawFlags) -> std::optional<std::string> {
  auto redraw = this->_needsRedraw;
  if (clearRedrawFlags) {
    this->_needsRedraw = std::nullopt;
  }

  return redraw;
}

void ViewManager::setNeedsUpdate(const std::string &reason) {
  this->_needsUpdate = this->_needsUpdate.value_or(reason);
  this->_needsRedraw = this->_needsRedraw.value_or(reason);
}

auto ViewManager::getViewports() -> std::list<std::shared_ptr<Viewport>> {
  if (this->_needsUpdate) {
    this->_update();
  }

  return this->_viewports;
}

auto ViewManager::getView(const std::string &viewId) -> std::shared_ptr<View> {
  // TODO(ib@unfolded.ai): Implement a lookup
  for (auto view : this->_views) {
    if (view->id == viewId) {
      return view;
    }
  }

  return nullptr;
}

auto ViewManager::getViewState(const std::string &viewId) -> std::shared_ptr<ViewState> { return this->_viewState; }

auto ViewManager::getViewport(const std::string &viewId) -> std::shared_ptr<Viewport> {
  return this->_viewportMap[viewId];
}

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

  // Only trigger an update if width actually changed
  if (width != this->_width) {
    this->_width = width;
    this->setNeedsUpdate("Window width changed");
  }
}

void ViewManager::setHeight(int height) {
  if (height < 0) {
    height = 0;
  }

  // Only trigger an update if height actually changed
  if (height != this->_height) {
    this->_height = height;
    this->setNeedsUpdate("Window height changed");
  }
}

void ViewManager::setViews(const std::list<std::shared_ptr<View>> &views) {
  auto viewsChanged = this->_diffViews(views, this->_views);
  if (viewsChanged) {
    this->_views = views;
    this->setNeedsUpdate("Views changed");
  }
}

void ViewManager::setViewState(std::shared_ptr<ViewState> viewState) {
  auto viewStateChanged = viewState && !viewState->equals(this->_viewState);

  if (viewStateChanged) {
    this->_viewState = viewState;
    this->setNeedsUpdate("viewState changed");
  }
}

void ViewManager::_update() {
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
  std::list<std::shared_ptr<Viewport>> viewports;
  auto rectangle = mathgl::Rectangle<int>{0, 0, this->_width, this->_height};

  for (auto const &view : this->getViews()) {
    auto viewState = this->getViewState(view->id);
    auto viewport = view->makeViewport(rectangle, viewState);

    viewports.push_back(viewport);
  }

  this->_viewports = viewports;

  this->_buildViewportMap();
}

void ViewManager::_buildViewportMap() {
  // Build a view id to view index
  this->_viewportMap = std::unordered_map<std::string, std::shared_ptr<Viewport>>{};
  for (const auto &viewport : this->_viewports) {
    // TODO(ib@unfolded.ai): Issue warning if multiple viewports use same id
    this->_viewportMap[viewport->id] = viewport;
  }
}

// Check if viewport array has changed, returns true if any change
// Note that descriptors can be the same
auto ViewManager::_diffViews(const std::list<std::shared_ptr<View>> &newViews,
                             const std::list<std::shared_ptr<View>> &oldViews) const -> bool {
  if (newViews.size() != oldViews.size()) {
    return true;
  }

  return !std::equal(oldViews.begin(), oldViews.end(), newViews.begin(), newViews.end(),
                     [](auto a, auto b) { return a.get()->equals(b); });
}
