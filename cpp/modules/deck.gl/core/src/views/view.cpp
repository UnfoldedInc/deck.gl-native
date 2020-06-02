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

const std::vector<const std::shared_ptr<Property>> propTypeDefs = {
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
  static auto properties = Properties::from<View>("View", propTypeDefs);
  return properties;
}

View::~View() {}

auto View::_getViewport(const Rectangle<int>& rect, std::shared_ptr<ViewState> viewState) const
    -> std::shared_ptr<Viewport> {
  // TODO(isaac)
  auto id = "my_viewport_idTODO";
  auto viewMatrixOptions = ViewMatrixOptions();
  auto projectionMatrixOptions = ProjectionMatrixOptions();
  return make_shared<Viewport>(id, viewMatrixOptions, projectionMatrixOptions, static_cast<double>(rect.x),
                               static_cast<double>(rect.y), static_cast<double>(rect.w), static_cast<double>(rect.h));
}

// Build a `Viewport` from a view descriptor
auto View::makeViewport(const Rectangle<int>& rect, shared_ptr<ViewState> viewState) -> shared_ptr<Viewport> {
  //   if (this->viewportInstance) {
  //     return this->viewportInstance;
  //   }
  // TODO(ib@unfolded.ai): calculate rect and filter view state
  // viewState = this->filterViewState(viewState);
  return this->_getViewport(rect, viewState);
}

// Build a `Viewport` from a view descriptor
// View::makeViewport({width, height, viewState}) {
//   if (this->viewportInstance) {
//     return this->viewportInstance;
//   }

//   viewState = this->filterViewState(viewState);

//   // Resolve relative viewport dimensions
//   const viewportDimensions = this->getDimensions({width, height});
//   const props = Object.assign({viewState}, viewState, this->props, viewportDimensions);
//   return this->_getViewport(props);

// View::getViewStateId() {
//   switch (typeof this->props.viewState) {
//     case 'string':
//       // if View.viewState is a string, return it
//       return this->props.viewState;

//     case 'object':
//       // If it is an object, return its id component
//       return this->props.viewState && this->props.viewState.id;

//     default:
//       return this->id;
//   }
// }

// Allows view to override (or completely define) viewState
// View::filterViewState(viewState) {
//   if (this->props.viewState&& typeof this->props.viewState == = 'object') {
//     // If we have specified an id, then intent is to override,
//     // If not, completely specify the view state
//     if (!this->props.viewState.id) {
//       return this->props.viewState;
//     }

//     // Merge in all props from View's viewState, except id
//     const newViewState = Object.assign({}, viewState);
//     for (const key in this->props.viewState) {
//       if (key != = 'id') {
//         newViewState[key] = this->props.viewState[key];
//       }
//     }
//     return newViewState;
//   }

//   return viewState;
// }

// Resolve relative viewport dimensions into actual dimensions (y='50%', width=800 => y=400)
// View::getDimensions({width, height}) {
//   return {
//     x : getPosition(this->_x, width),
//     y : getPosition(this->_y, height),
//     width : getPosition(this->_width, width),
//     height : getPosition(this->_height, height)
//   };
// }

// Parse relative viewport dimension descriptors (e.g {y: '50%', height: '50%'})
// View::_parseDimensions({x, y, width, height}) {
//   this->_x = parsePosition(x);
//   this->_y = parsePosition(y);
//   this->_width = parsePosition(width);
//   this->_height = parsePosition(height);
// }
