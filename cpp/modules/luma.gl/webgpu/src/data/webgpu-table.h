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

#ifndef LUMAGL_CORE_WEBGPU_TABLE_H
#define LUMAGL_CORE_WEBGPU_TABLE_H

#include <memory>
#include <vector>

#include "./webgpu-column.h"

namespace lumagl {

class WebGPUTable {
 public:
  explicit WebGPUTable(int64_t numRows, const std::vector<std::shared_ptr<lumagl::WebGPUColumn>>& columns)
      : numRows{numRows}, columns{columns} {};

  int64_t numRows{0};
  std::vector<std::shared_ptr<lumagl::WebGPUColumn>> columns;
};

}  // namespace lumagl

#endif  // LUMAGL_CORE_WEBGPU_TABLE_H