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

#ifndef DECKGL_CORE_VIEWS_VIEW_STATE_H
#define DECKGL_CORE_VIEWS_VIEW_STATE_H

#include <optional>

#include "deck.gl/json.h"

namespace deckgl {

class ViewState : public Component {
 public:
  using super = Component;
  class Props;

  virtual ~ViewState() {}
};

class ViewState::Props : public Component::Props {
 public:
  static constexpr const char* getTypeName() { return "ViewState"; }
  // Map view states
  std::optional<double> longitude;
  std::optional<double> latitude;
  std::optional<double> zoom;
  std::optional<double> bearing;
  std::optional<double> pitch;
};

}  // namespace deckgl

#endif  // DECKGL_CORE_VIEWS_VIEW_STATE_H
