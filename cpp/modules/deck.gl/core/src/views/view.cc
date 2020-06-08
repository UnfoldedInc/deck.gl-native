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

#include "./view.h"  // NOLINT(build/include)

#include <vector>

using namespace std;
using namespace deckgl;
using namespace mathgl;

const std::vector<std::shared_ptr<Property>> propTypeDefs = {
    std::make_shared<PropertyT<string>>(
        "id", [](const JSONObject* props) { return dynamic_cast<const View*>(props)->id; },
        [](JSONObject* props, string value) { return dynamic_cast<View*>(props)->id = value; }, ""),
    std::make_shared<PropertyT<int>>(
        "x", [](const JSONObject* props) { return dynamic_cast<const View*>(props)->x; },
        [](JSONObject* props, int value) { return dynamic_cast<View*>(props)->x = value; }, 0),
    std::make_shared<PropertyT<int>>(
        "y", [](const JSONObject* props) { return dynamic_cast<const View*>(props)->y; },
        [](JSONObject* props, int value) { return dynamic_cast<View*>(props)->y = value; }, 0),
    std::make_shared<PropertyT<int>>(
        "width", [](const JSONObject* props) { return dynamic_cast<const View*>(props)->width; },
        [](JSONObject* props, int value) { return dynamic_cast<View*>(props)->width = value; }, 100),
    std::make_shared<PropertyT<int>>(
        "height", [](const JSONObject* props) { return dynamic_cast<const View*>(props)->height; },
        [](JSONObject* props, int value) { return dynamic_cast<View*>(props)->height = value; }, 100),

    std::make_shared<PropertyT<double>>(
        "fovy", [](const JSONObject* props) { return dynamic_cast<const View*>(props)->fovy; },
        [](JSONObject* props, double value) { return dynamic_cast<View*>(props)->fovy = value; }, 1.0),
    std::make_shared<PropertyT<double>>(
        "near", [](const JSONObject* props) { return dynamic_cast<const View*>(props)->near; },
        [](JSONObject* props, double value) { return dynamic_cast<View*>(props)->near = value; }, 0.0),
    std::make_shared<PropertyT<double>>(
        "far", [](const JSONObject* props) { return dynamic_cast<const View*>(props)->far; },
        [](JSONObject* props, double value) { return dynamic_cast<View*>(props)->far = value; },
        std::numeric_limits<double>::max())};

auto View::getProperties() const -> const std::shared_ptr<Properties> {
  static auto properties = Properties::from<View>(propTypeDefs);
  return properties;
}

View::~View() {}

auto View::makeViewport(const Rectangle<int>& rect, const shared_ptr<ViewState>& viewState) -> shared_ptr<Viewport> {
  //   if (this->viewportInstance) {
  //     return this->viewportInstance;
  //   }
  // TODO(ib@unfolded.ai): calculate rect and filter view state
  // viewState = this->filterViewState(viewState);
  return this->_getViewport(rect, viewState);
}

auto View::_getViewport(const Rectangle<int>& rect, const std::shared_ptr<ViewState>& viewState) const
    -> std::shared_ptr<Viewport> {
  // TODO(isaac)
  auto id = "my_viewport_idTODO";
  auto viewMatrixOptions = ViewMatrixOptions();
  auto projectionMatrixOptions = ProjectionMatrixOptions();
  return make_shared<Viewport>(id, viewMatrixOptions, projectionMatrixOptions, static_cast<double>(rect.x),
                               static_cast<double>(rect.y), static_cast<double>(rect.w), static_cast<double>(rect.h));
}
