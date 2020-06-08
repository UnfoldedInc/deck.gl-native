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

#include "../lib/component.h"
#include "../viewports/viewport.h"
#include "./view-state.h"
#include "deck.gl/json.h"
#include "math.gl/core.h"

namespace deckgl {

class ViewManager;

/// \brief View represents a cutomizable point of view that can be rendered.
class View : public Component::Props {
  friend class ViewManager;

 public:
  using super = Component::Props;

  virtual ~View();

  /// \brief Builds a Viewport from a given viewState.
  /// \param rect Rectangle that represents the origin and size of the Viewport.
  /// \return A new Viewport instance, based on given arguments.
  auto makeViewport(const mathgl::Rectangle<int>& rect, const std::shared_ptr<ViewState>& viewState)
      -> std::shared_ptr<Viewport>;

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
  static constexpr const char* getTypeName() { return "View"; }
  auto getProperties() const -> const std::shared_ptr<Properties> override;

 protected:
  // Create actual viewport
  virtual auto _getViewport(const mathgl::Rectangle<int>& rect, const std::shared_ptr<ViewState>& viewState) const
      -> std::shared_ptr<Viewport>;

 private:
  // Allows view to override (or completely define) viewState
  auto filterViewState(std::shared_ptr<ViewState>) -> std::shared_ptr<ViewState>;
};

}  // namespace deckgl

#endif  // DECKGL_CORE_VIEWS_VIEW_H
