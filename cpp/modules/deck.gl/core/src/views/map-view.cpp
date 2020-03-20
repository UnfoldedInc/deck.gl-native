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

#include "./map-view.h"                          // {MapView}
#include "../viewports/web-mercator-viewport.h"  // {WebMercatorViewport}

#include <vector>

using namespace std;
using namespace deckgl;
using namespace mathgl;

auto MapView::_getViewport(const Rectangle<int>& rect, shared_ptr<ViewState> viewState) const -> shared_ptr<Viewport> {
  return make_shared<WebMercatorViewport>(rect.w, rect.h, viewState->longitude.value_or(0),
                                          viewState->latitude.value_or(0), viewState->zoom.value_or(11),
                                          viewState->pitch.value_or(0), viewState->bearing.value_or(0));
}
