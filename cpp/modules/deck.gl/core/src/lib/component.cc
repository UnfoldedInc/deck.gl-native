// Copyright (c) 2020 Unfolded, Inc.
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

#include "./component.h"  // NOLINT(build/include)

using namespace deckgl;

// Setters and getters for properties
// TODO(ib@unfolded.ai): auto generate from language-independent prop definition schema
// TODO(ilija@unfolded.ai): Generate a unique id instead of an empty string for default value?
static const std::vector<std::shared_ptr<Property>> propTypeDefs = {std::make_shared<PropertyT<std::string>>(
    "id", [](const JSONObject* props) { return dynamic_cast<const Component::Props*>(props)->id; },
    [](JSONObject* props, std::string value) { return dynamic_cast<Component::Props*>(props)->id = value; }, "")};

auto Component::Props::getProperties() const -> const std::shared_ptr<Properties> {
  static auto properties = Properties::from<Component::Props>(propTypeDefs);
  return properties;
}
