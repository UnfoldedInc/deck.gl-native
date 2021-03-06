// Copyright (c) 2020 Unfolded Inc.
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

#ifndef LUMAGL_CORE_SIZE_H
#define LUMAGL_CORE_SIZE_H

namespace lumagl {

struct Size {
 public:
  Size() : width{0}, height{0} {}
  Size(int width, int height) : width{width}, height{height} {}

  template <typename T>
  auto operator*(const T& value) -> Size {
    return Size{static_cast<int>(this->width * value), static_cast<int>(this->height * value)};
  }

  int width;
  int height;
};

}  // namespace lumagl

auto operator==(const lumagl::Size& s1, const lumagl::Size& s2) -> bool;
auto operator!=(const lumagl::Size& s1, const lumagl::Size& s2) -> bool;

#endif  // LUMAGL_CORE_SIZE_H
