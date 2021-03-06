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

#include "./view-state.h"  // NOLINT(build/include)

using namespace deckgl;

const std::vector<std::shared_ptr<Property>> propTypeDefs = {
    std::make_shared<PropertyT<std::optional<double>>>(
        "longitude", [](const JSONObject* props) { return dynamic_cast<const ViewState*>(props)->longitude; },
        [](JSONObject* props, const double& value) { return dynamic_cast<ViewState*>(props)->longitude = value; }, 0.0),
    std::make_shared<PropertyT<std::optional<double>>>(
        "latitude", [](const JSONObject* props) { return dynamic_cast<const ViewState*>(props)->latitude; },
        [](JSONObject* props, const double& value) { return dynamic_cast<ViewState*>(props)->latitude = value; }, 0.0),
    std::make_shared<PropertyT<std::optional<double>>>(
        "zoom", [](const JSONObject* props) { return dynamic_cast<const ViewState*>(props)->zoom; },
        [](JSONObject* props, const double& value) { return dynamic_cast<ViewState*>(props)->zoom = value; }, 10),
    std::make_shared<PropertyT<std::optional<double>>>(
        "bearing", [](const JSONObject* props) { return dynamic_cast<const ViewState*>(props)->bearing; },
        [](JSONObject* props, const double& value) { return dynamic_cast<ViewState*>(props)->bearing = value; }, 0.0),
    std::make_shared<PropertyT<std::optional<double>>>(
        "pitch", [](const JSONObject* props) { return dynamic_cast<const ViewState*>(props)->pitch; },
        [](JSONObject* props, const double& value) { return dynamic_cast<ViewState*>(props)->pitch = value; }, 0.0)};

auto ViewState::getProperties() const -> const std::shared_ptr<Properties> {
  static auto properties = Properties::from<ViewState>(propTypeDefs);
  return properties;
}
