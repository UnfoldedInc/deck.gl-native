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

#ifndef DECKGL_CORE_VIEWS_VIEW_MANAGER_H
#define DECKGL_CORE_VIEWS_VIEW_MANAGER_H

#include <list>
#include <memory>
#include <optional>
#include <string>

#include "../viewports/viewport.h"
#include "../views/view-state.h"
#include "../views/view.h"

namespace deckgl {

class ViewManager {
 public:
  ViewManager();
  virtual ~ViewManager();

  // Check if a redraw is needed
  auto getNeedsRedraw(bool clearRedrawFlags = false) -> std::optional<std::string>;

  // Views will be updated deeply (in next animation frame)
  void setNeedsUpdate(const std::string &reason);

  // Checks each viewport for transition updates
  void updateViewStates();

  /** Get a set of viewports for a given width and height
   * TODO - Intention is for deck.gl to autodeduce width and height and drop
   * the need for props
   */
  auto getViewports() -> std::list<std::shared_ptr<Viewport>>;  // (rect)

  auto getViews() -> std::list<std::shared_ptr<View>>;

  // Resolves a viewId string to a View, if already a View returns it.
  auto getView(const std::string &viewOrViewId) -> std::shared_ptr<View>;

  // Returns the viewState for a specific viewId. Matches the viewState by
  // 1. view.viewStateId
  // 2. view.id
  // 3. root viewState
  // then applies the view's filter if any
  auto getViewState(const std::string &viewId) -> std::shared_ptr<ViewState>;
  auto getViewport(const std::string &viewId) -> std::shared_ptr<Viewport>;

  /**
   * Unproject pixel coordinates on screen onto world coordinates,
   * (possibly [lon, lat]) on map.
   * - [x, y] => [lng, lat]
   * - [x, y, z] => [lng, lat, Z]
   * @param xyz -
   * @param topLeft - Whether origin is top left
   * @return {Array|null} - [lng, lat, Z] or [X, Y, Z]
   */
  auto unproject(const mathgl::Vector3<double> xyz, bool topLeft = true) -> std::optional<mathgl::Vector3<double>>;
  auto unproject(const mathgl::Vector2<double> xy, bool topLeft = true) -> std::optional<mathgl::Vector2<double>>;

  //
  // MODIFIERS
  //

  // Set the size of the window
  void setSize(int width, int height);
  void setWidth(int width);
  void setHeight(int height);

  // Update the view descriptor list (Does not rebuild the `Viewport`s until `getViewports` is called)
  void setViews(const std::list<std::shared_ptr<View>> &views);

  // Update the view state
  void setViewState(std::shared_ptr<ViewState> viewStates);
  void setViewStates(const std::list<std::shared_ptr<ViewState>> &viewStates);

  std::list<std::shared_ptr<View>> views;
  int width{100};
  int height{100};
  std::shared_ptr<ViewState> viewState{new ViewState()};

 private:
  void _update();

  // Rebuilds viewports from descriptors towards a certain window size
  void _rebuildViewports();

  /*
  void _onViewStateChange(viewId, event);

  void _createController(props);

  void _updateController(view, viewState, viewport, controller);

  void _buildViewportMap();
  */

  // Check if viewport array has changed, returns true if any change
  // Note that descriptors can be the same
  auto _diffViews(const std::list<std::shared_ptr<View>> &newViews,
                  const std::list<std::shared_ptr<View>> &oldViews) const -> bool;

  std::optional<std::string> _needsRedraw{"Initial render"};
  std::optional<std::string> _needsUpdate{"Initial render"};
  std::list<std::shared_ptr<Viewport>> _viewports;  // Generated viewports
  bool _isUpdating{false};
};

}  // namespace deckgl

#endif  // DECKGL_CORE_VIEWS_VIEW_MANAGER_H
