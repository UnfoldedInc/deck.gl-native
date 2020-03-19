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

#include "./view-state.h"

using namespace deckgl;

const std::vector<const Property*> propTypeDefs = {
    new PropertyT<std::optional<float>>{
        "longitude", [](const JSONObject* props) { return dynamic_cast<const ViewState*>(props)->longitude; },
        [](JSONObject* props, const float& value) { return dynamic_cast<ViewState*>(props)->longitude = value; }, 0.0},
    new PropertyT<std::optional<float>>{
        "latitude", [](const JSONObject* props) { return dynamic_cast<const ViewState*>(props)->latitude; },
        [](JSONObject* props, const float& value) { return dynamic_cast<ViewState*>(props)->latitude = value; }, 0.0},
    new PropertyT<std::optional<float>>{
        "zoom", [](const JSONObject* props) { return dynamic_cast<const ViewState*>(props)->zoom; },
        [](JSONObject* props, const float& value) { return dynamic_cast<ViewState*>(props)->zoom = value; }, 10}};

auto ViewState::getProperties() const -> const Properties* {
  static Properties properties{Properties::from<ViewState>("ViewState", propTypeDefs)};
  return &properties;
}