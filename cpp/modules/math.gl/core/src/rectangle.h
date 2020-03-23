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

#ifndef MATHGL_CORE_RECTANGLE_H
#define MATHGL_CORE_RECTANGLE_H

#include <ostream>

namespace mathgl {

template <typename coord>
class Rectangle {
 public:
  Rectangle() : x(0), y(0), w(0), h(0) {}
  Rectangle(coord x1, coord y1, coord w1, coord h1) : x(x1), y(y1), w(w1), h(h1) {}

  auto operator==(const Rectangle<coord> &r) const -> bool { return x == r.x && y == r.y && w == r.w && h == r.h; }

  coord x, y, w, h;
};

template <typename coord>
auto operator<<(std::ostream &os, const Rectangle<coord> &r) -> std::ostream & {
  os << "Rect(x=" << r.x << ", y=" << r.y << ", width=" << r.w << ", height=" << r.h << ")";
  return os;
}

}  // namespace mathgl

#endif
