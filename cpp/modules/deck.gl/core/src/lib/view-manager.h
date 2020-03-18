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

namespace deckgl {

class ViewManager {
 public:
  ViewManager();
  virtual ~ViewManager();

  // Check if a redraw is needed
  needsRedraw(bool clearRedrawFlags = false) {
    const redraw = this->_needsRedraw;
    if (opts.clearRedrawFlags) {
      this->_needsRedraw = false;
    }
    return redraw;
  }

  // Views will be updated deeply (in next animation frame)
  // Potentially regenerating attributes and sub layers
  void setNeedsUpdate(const std::string &reason);

  // Checks each viewport for transition updates
  void updateViewStates();

  /** Get a set of viewports for a given width and height
   * TODO - Intention is for deck.gl to autodeduce width and height and drop
   * the need for props
   * @param rect (object, optional) - filter the viewports
   *   + not provided - return all viewports
   *   + {x, y} - only return viewports that contain this pixel
   *   + {x, y, width, height} - only return viewports that overlap with this
   * rectangle
   */
  auto getViewports() -> void;  // (rect)

  auto getViews() -> void;

  // Resolves a viewId string to a View, if already a View returns it.
  auto getView(const std::string &viewOrViewId) -> void;

  // Returns the viewState for a specific viewId. Matches the viewState by
  // 1. view.viewStateId
  // 2. view.id
  // 3. root viewState
  // then applies the view's filter if any
  auto getViewState(const std::string &viewId) -> void;
  auto getViewport(viewId) -> std::shared_ptr<Viewport>;

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
  // unproject(xyz, opts);

  void setProps(void *props);

  //
  // PRIVATE METHODS
  //

  /*
  void _update();

  void _setSize(width, height);

  // Update the view descriptor list and set change flag if needed
  // Does not actually rebuild the `Viewport`s until `getViewports` is called
  void _setViews(views);

  void _setViewState(viewState);

  void _onViewStateChange(viewId, event);

  void _createController(props);

  void _updateController(view, viewState, viewport, controller);

  // Rebuilds viewports from descriptors towards a certain window size
  void _rebuildViewports();

  void _buildViewportMap();

  // Check if viewport array has changed, returns true if any change
  // Note that descriptors can be the same
  void _diffViews(newViews, oldViews);
  */
}

}  // namespace deckgl

#endif  // DECKGL_CORE_VIEWS_VIEW_MANAGER_H
