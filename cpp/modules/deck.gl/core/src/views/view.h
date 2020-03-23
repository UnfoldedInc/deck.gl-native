// Copyright (c) 2020, Unfolded Inc
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

#ifndef DECKGL_CORE_VIEWS_VIEW_H
#define DECKGL_CORE_VIEWS_VIEW_H

#include <memory>
#include <string>

#include "../lib/component.h"       // {Component}
#include "../viewports/viewport.h"  // {Viewport}
#include "./view-state.h"           // {ViewState}
#include "deck.gl/json.h"
#include "math.gl/core.h"

namespace deckgl {

class ViewManager;

class View : public Component {
  friend class ViewManager;

 public:
  class Props;

  explicit View(std::shared_ptr<View::Props> props);
  ~View();

  bool equals(const View *view);

  auto getViewStateId() const -> std::string;

  // Build a `Viewport` from a view descriptor
  auto makeViewport(const mathgl::Rectangle<int> &rect, std::shared_ptr<ViewState> viewState)
      -> std::shared_ptr<Viewport>;

 protected:
  // Create actual viewport
  virtual auto _getViewport(const mathgl::Rectangle<int> &rect, std::shared_ptr<ViewState> viewState) const
      -> std::shared_ptr<Viewport>;

 private:
  // Allows view to override (or completely define) viewState
  auto filterViewState(std::shared_ptr<ViewState>) -> std::shared_ptr<ViewState>;

  // Resolve relative viewport dimensions into actual dimensions (y='50%', width=800 => y=400)
  // auto getDimensions(int width, int height) -> Rect...

  // Parse relative viewport dimension descriptors (e.g {y: '50%', height: '50%'})
  // _parseDimensions(int x, int y, int width, int height});
};

// TODO(ib) - how do we override viewstate? inherit from ViewState
class View::Props : public Component::Props {
 public:
  using super = Component::Props;

  std::string id;

  // width/height of view
  int x{0};
  int y{0};
  int width{100};
  int height{100};

  // Viewport Options
  std::optional<mathgl::Matrix4<double>> projectionMatrix;  // Projection matrix
  std::optional<mathgl::Matrix4<double>> modelMatrix;       // A model matrix to be applied to position

  // Perspective projection parameters, used if projectionMatrix not supplied
  double fovy{50};
  double near{0.1};  // Distance of near clipping plane
  double far{1000};  // Distance of far clipping plane

  // Property type machinery
  static constexpr const char *getTypeName() { return "View"; }
  auto getProperties() const -> const Properties * override;
  auto makeComponent(std::shared_ptr<Component::Props> props) const -> View * override {
    return new View{std::dynamic_pointer_cast<View::Props>(props)};
  }
};

}  // namespace deckgl

#endif  // DECKGL_CORE_VIEWS_VIEW_H
