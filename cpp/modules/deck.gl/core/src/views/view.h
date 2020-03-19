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

#include "../viewports/viewport.h"  // {Viewport}

namespace deckgl {

class View : public Component {
 public:
  class Props;

  View(Props *props);
  ~View() {}

  bool equals(const View *view);

  // Build a `Viewport` from a view descriptor
  // TODO - add support for autosizing viewports using width and height
  auto makeViewport() -> Viewport *;  // {width, height, viewState});

  auto getViewStateId() -> const std::string;

  // Allows view to override (or completely define) viewState
  // filterViewState(viewState);

  // Resolve relative viewport dimensions into actual dimensions (y='50%', width=800 => y=400)
  // auto getDimensions(int width, int height) -> ;

  // Used by sub classes to resolve controller props
  // _getControllerProps(void *defaultOpts);

  // Overridable method
  // _getViewport(void *props);

  // Parse relative viewport dimension descriptors (e.g {y: '50%', height: '50%'})
  // _parseDimensions(int x, int y, int width, int height});
};

class View::Props : public Component::Props {
 public:
  static constexpr const char *getTypeName() { return "View"; }

  std::string id;

  // Window width/height in pixels (for pixel projection)
  int x{0};
  int y{0};
  const std::string width{"100%"};
  const std::string height{"100%"};

  // Viewport Options
  mathgl::Matrix4<double> projectionMatrix;  // Projection matrix
  mathgl::Matrix4<double> modelMatrix;       // A model matrix to be applied to position, to match the layer props API

  // Perspective projection parameters, used if projectionMatrix not supplied
  double fovy{50};
  double near{0.1};  // Distance of near clipping plane
  double far{1000};  // Distance of far clipping plane

  // A View can be a wrapper for a viewport instance
  // viewportInstance = null,

  // Internal: Viewport Type
  // type = Viewport // TODO - default to WebMercator?
};

}  // namespace deckgl

#endif  // DECKGL_CORE_VIEWS_VIEW_H
