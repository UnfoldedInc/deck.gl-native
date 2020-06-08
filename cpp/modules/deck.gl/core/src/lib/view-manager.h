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
#include <unordered_map>

#include "../viewports/viewport.h"
#include "../views/view-state.h"
#include "../views/view.h"

namespace deckgl {

/// \brief The ViewManager class manages a set of views' lifecycle.
class ViewManager {
 public:
  ViewManager();
  virtual ~ViewManager();

  /// \brief Check if a redraw is needed.
  /// \param clearRedrawFlags Whether or not to clear the current redraw flags.
  /// \return Returns a reason for redraw, if one exists.
  auto getNeedsRedraw(bool clearRedrawFlags = false) -> std::optional<std::string>;

  /// \brief Views will be updated deeply (in next animation frame).
  /// \param reason Reason of the redraw.
  void setNeedsUpdate(const std::string& reason);

  /// \brief Get all the viewports for views being managed.
  auto getViewports() -> std::list<std::shared_ptr<Viewport>>;

  /// Returns all the views currently being managed.
  auto getViews() -> std::list<std::shared_ptr<View>> { return this->_views; };

  /// \brief Resolves a viewId string to a View, if already a View returns it.
  /// \param viewId Identifier of the view to get.
  /// \return View represented by viewId, if one was found.
  auto getView(const std::string& viewId) -> std::shared_ptr<View>;

  /// \brief Returns the viewState for a specific viewId.
  /// Matches the viewState by:
  ///   1. view.viewStateId
  ///   2. view.id
  ///   3. root viewState
  /// then applies the view's filter if any
  auto getViewState(const std::string& viewId) -> std::shared_ptr<ViewState>;
  auto getViewport(const std::string& viewId) -> std::shared_ptr<Viewport>;

  /// \brief Unproject pixel coordinates on screen onto world coordinates, (possibly [lon, lat]) on map.
  /// - [x, y] => [lng, lat]
  /// - [x, y, z] => [lng, lat, Z]
  /// @param xy Pixel coordinates
  /// @param topLeft - Whether origin is top left
  /// @return [lng, lat, Z] or [X, Y, Z]
  auto unproject(const mathgl::Vector2<double> xy, bool topLeft = true) -> std::optional<mathgl::Vector2<double>>;
  /// \overload
  auto unproject(const mathgl::Vector3<double> xyz, bool topLeft = true) -> std::optional<mathgl::Vector3<double>>;

  //
  // MODIFIERS
  //

  /// \brief Set the size of the window.
  void setSize(int width, int height);

  auto width() -> int { return this->_width; }
  void setWidth(int width);

  auto height() -> int { return this->_height; }
  void setHeight(int height);

  /// \brief Update the view descriptor list (Does not rebuild the `Viewport`s until `getViewports` is called).
  void setViews(const std::list<std::shared_ptr<View>>& views);

  /// \brief Update the view state.
  void setViewState(std::shared_ptr<ViewState> viewStates);

 private:
  void _update();

  /// \brief Rebuilds viewports from descriptors towards a certain window size.
  void _rebuildViewports();
  void _buildViewportMap();

  // Check if viewport array has changed, returns true if any change
  // Note that descriptors can be the same
  auto _diffViews(const std::list<std::shared_ptr<View>>& newViews,
                  const std::list<std::shared_ptr<View>>& oldViews) const -> bool;

  std::list<std::shared_ptr<View>> _views;
  int _width{100};
  int _height{100};
  std::shared_ptr<ViewState> _viewState{new ViewState()};

  std::optional<std::string> _needsRedraw{"Initial render"};
  std::optional<std::string> _needsUpdate{"Initial render"};
  std::list<std::shared_ptr<Viewport>> _viewports;
  std::unordered_map<std::string, std::shared_ptr<Viewport>> _viewportMap;
  bool _isUpdating{false};
};

}  // namespace deckgl

#endif  // DECKGL_CORE_VIEWS_VIEW_MANAGER_H
