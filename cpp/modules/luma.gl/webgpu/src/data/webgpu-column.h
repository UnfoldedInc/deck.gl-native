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

#ifndef LUMAGL_CORE_WEBGPU_COLUMN_H
#define LUMAGL_CORE_WEBGPU_COLUMN_H

#include <arrow/array.h>
#include <dawn/webgpu_cpp.h>

#include <memory>

#include "luma.gl/webgpu/src/webgpu-helpers.h"

namespace lumagl {

class WebGPUColumn {
 public:
  WebGPUColumn(wgpu::Device device, const std::shared_ptr<arrow::Array>& column);
  ~WebGPUColumn();

  void setData(const std::shared_ptr<arrow::Array>& column);

  wgpu::Buffer buffer;
  /// \brief Size of a single element, in bytes, that this buffer contains.
  size_t elementSize{0};
};

}  // namespace lumagl

#endif  // LUMAGL_CORE_WEBGPU_COLUMN_H
