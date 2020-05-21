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

#ifndef DECKGL_CORE_VIEWS_MAP_VIEW_H
#define DECKGL_CORE_VIEWS_MAP_VIEW_H

#include <memory>

#include "./view.h"  // {View}
// #include "../viewports/web-mercator-viewport.h" // {WebMercatorViewport}
#include "math.gl/core.h"

namespace deckgl {

class MapView : public View {
 public:
  using super = View;

  static constexpr const char* getTypeName() { return "MapView"; }
  auto getProperties() const -> const Properties* override;

 protected:
  auto _getViewport(const mathgl::Rectangle<int>& rect, std::shared_ptr<ViewState> viewState) const
      -> std::shared_ptr<Viewport> override;
};

}  // namespace deckgl

#endif  // DECKGL_CORE_VIEWS_MAP_VIEW_H
