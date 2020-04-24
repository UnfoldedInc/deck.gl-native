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

#ifndef DECKGL_LAYERS_SOLIDPOLYGON_LAYER_H
#define DECKGL_LAYERS_SOLIDPOLYGON_LAYER_H

#include <memory>
#include <string>

#include "deck.gl/core.h"

namespace deckgl {

class SolidPolygonLayer : public Layer{
 public:
    class Props;
    explicit SolidPolygonLayer(std::shared_ptr<SolidPolygonLayer::Props> props)
      : Layer{std::dynamic_pointer_cast<Layer::Props>(props)} {}
    auto props() { return std::dynamic_pointer_cast<Layer::Props>(this->_props); }
 protected:
 private:
};

class SolidPolygonLayer::Props : public Layer::Props {
 public:
 protected:
 private:
};

}  // namespace deckgl
#endif  // DECKGL_LAYERS_SOLIDPOLYGON_LAYER_H
