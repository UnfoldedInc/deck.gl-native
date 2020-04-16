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
#include <vector>

#include "luma.gl/webgpu/src/data/attribute-descriptor.h"
#include "luma.gl/webgpu/src/webgpu-helpers.h"

namespace lumagl {

/// \brief Array data structure that manages the backing GPU buffer.
class WebGPUArray {
 public:
  WebGPUArray(wgpu::Device device, const std::shared_ptr<AttributeDescriptor>& descriptor)
      : _device{device}, _descriptor{descriptor} {}
  WebGPUArray(wgpu::Device device, const std::shared_ptr<AttributeDescriptor>& descriptor,
              const std::shared_ptr<arrow::Array>& data);
  template <typename T>
  WebGPUArray(wgpu::Device device, const std::shared_ptr<AttributeDescriptor>& descriptor, const std::vector<T>& data)
      : WebGPUArray{device, descriptor} {
    this->setData(data);
  }
  ~WebGPUArray();

  // TODO(ilija@unfolded.ai): Is array/buffer mutability something we'll be making use of?
  void setData(const std::shared_ptr<arrow::Array>& data);
  template <typename T>
  void setData(const std::vector<T>& data) {
    auto bufferSize = this->_descriptor->size() * data->size();
    if (!this->_buffer || data->size() != this->_length) {
      // TODO(ilija@unfolded.ai): Should usage be part of the descriptor?
      this->_buffer =
          this->_createBuffer(this->_device, bufferSize, wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Vertex);
    }

    this->_buffer.SetSubData(0, bufferSize, data.data());
    this->_length = data.size();
  }

  /// \brief Returns the backing buffer that this array manages.
  auto buffer() const -> wgpu::Buffer;

  /// \brief Size in the number of elements this array contains.
  auto length() const -> int64_t;

 private:
  auto _createBuffer(wgpu::Device device, uint64_t size, wgpu::BufferUsage usage) -> wgpu::Buffer;

  wgpu::Device _device;
  std::shared_ptr<AttributeDescriptor> _descriptor;

  wgpu::Buffer _buffer{nullptr};
  int64_t _length{0};
};

}  // namespace lumagl

#endif  // LUMAGL_WEBGPU_WEBGPU_COLUMN_H
