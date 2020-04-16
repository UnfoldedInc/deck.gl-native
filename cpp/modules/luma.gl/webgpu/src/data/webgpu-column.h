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

#ifndef LUMAGL_WEBGPU_WEBGPU_COLUMN_H
#define LUMAGL_WEBGPU_WEBGPU_COLUMN_H

#include <arrow/array.h>
#include <dawn/webgpu_cpp.h>

#include <memory>

#include "luma.gl/webgpu/src/data/attribute-descriptor.h"
#include "luma.gl/webgpu/src/webgpu-helpers.h"

namespace lumagl {

class WebGPUColumn {
 public:
  WebGPUColumn(wgpu::Device device, const std::shared_ptr<AttributeDescriptor>& descriptor);
  ~WebGPUColumn();

  void setData(const std::shared_ptr<arrow::Array>& data);

  wgpu::Buffer buffer;
  /// \brief Size in the number of elements this column contains.
  int64_t length{0};

 private:
  auto _createBuffer(wgpu::Device device, uint64_t size, wgpu::BufferUsage usage) -> wgpu::Buffer;

  wgpu::Device _device;
  std::shared_ptr<AttributeDescriptor> _descriptor;
};

}  // namespace lumagl

#endif  // LUMAGL_WEBGPU_WEBGPU_COLUMN_H
