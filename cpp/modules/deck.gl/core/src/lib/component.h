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

#ifndef DECKGL_CORE_LIB_COMPONENT_H
#define DECKGL_CORE_LIB_COMPONENT_H

#include <memory>

#include "deck.gl/json.h"  // {JSONObject}

namespace deckgl {

class Component {
 public:
  class Props;
  explicit Component(std::shared_ptr<Props> props) : _props{props} {}
  virtual ~Component() {}

 protected:
  std::shared_ptr<Props> _props;
};

class Component::Props : public JSONObject {
 public:
  using super = JSONObject;
  virtual auto makeComponent(std::shared_ptr<Component::Props> props) const -> std::shared_ptr<Component> {
    return std::make_shared<Component>(std::dynamic_pointer_cast<Component::Props>(props));
  }
};

}  // namespace deckgl

#endif  // DECKGL_CORE_LIB_COMPONENT_H
