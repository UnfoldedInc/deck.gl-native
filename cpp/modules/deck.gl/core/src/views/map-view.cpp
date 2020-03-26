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

#include "./map-view.h"  // {MapView} NOLINT(build/include)

#include <vector>

#include "../viewports/web-mercator-viewport.h"  // {WebMercatorViewport}

using namespace std;
using namespace deckgl;
using namespace mathgl;

auto MapView::getProperties() const -> const Properties* {
  static Properties properties{Properties::from<MapView>("MapView", super::getProperties()->allProperties())};
  return &properties;
}

auto MapView::_getViewport(const Rectangle<int>& rect, shared_ptr<ViewState> viewState) const -> shared_ptr<Viewport> {
  WebMercatorViewport::Options opts;
  opts.width = rect.w;
  opts.height = rect.h;
  if (viewState->longitude) {
    opts.longitude = viewState->longitude.value();
  }
  if (viewState->latitude) {
    opts.latitude = viewState->latitude.value();
  }
  if (viewState->zoom) {
    opts.zoom = viewState->zoom.value();
  }
  if (viewState->pitch) {
    opts.pitch = viewState->pitch.value();
  }
  if (viewState->bearing) {
    opts.bearing = viewState->bearing.value();
  }

  return make_shared<WebMercatorViewport>(opts);
}
