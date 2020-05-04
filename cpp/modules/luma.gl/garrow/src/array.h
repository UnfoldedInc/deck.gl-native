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

#ifndef LUMAGL_GARROW_ARRAY_H
#define LUMAGL_GARROW_ARRAY_H

#include <arrow/array.h>
#include <dawn/webgpu_cpp.h>

#include <memory>
#include <utility>
#include <vector>

namespace lumagl {
namespace garrow {

/// \brief Array data structure that manages the backing GPU buffer.
class Array {
 public:
  // TODO(ilija@unfolded.ai): In arrow, Array is an abstract class with typed implementaions that gets created using
  // the builder API
  explicit Array(wgpu::Device device) : _device{device} {}
  Array(wgpu::Device device, const std::shared_ptr<arrow::Array>& data, wgpu::BufferUsage usage);
  template <typename T>
  Array(wgpu::Device device, const std::vector<T>& data, wgpu::BufferUsage usage) : Array{device} {
    this->setData(data.data(), data.size(), usage);
  }
  template <typename T>
  Array(wgpu::Device device, const T* data, size_t length, wgpu::BufferUsage usage) : Array{device} {
    this->setData(data, length, usage);
  }
  ~Array();

  /// \brief Size in the number of elements this array contains.
  auto length() const -> int64_t { return this->_length; };

  /* Arrow non-compliant API */

  // TODO(ilija@unfolded.ai): Arrays are not mutable in Arrow. Revisit once ArrayData is in place
  void setData(const std::shared_ptr<arrow::Array>& data, wgpu::BufferUsage usage);
  template <typename T>
  void setData(const T* data, size_t length, wgpu::BufferUsage usage) {
    auto bufferByteSize = sizeof(T) * length;
    if (!this->_buffer || bufferByteSize != this->_bufferByteSize) {
      this->_buffer = this->_createBuffer(this->_device, bufferByteSize, usage);
    }

    this->_buffer.SetSubData(0, bufferByteSize, data);
    this->_length = length;
    this->_bufferByteSize = bufferByteSize;
  }

  /// \brief Returns the backing buffer that this array manages.
  auto buffer() const -> wgpu::Buffer { return this->_buffer; };

 private:
  auto _createBuffer(wgpu::Device device, uint64_t size, wgpu::BufferUsage usage) -> wgpu::Buffer;

  wgpu::Device _device;
  wgpu::Buffer _buffer{nullptr};
  int64_t _length{0};
  uint64_t _bufferByteSize{0};
};

}  // namespace garrow
}  // namespace lumagl

#endif  // LUMAGL_GARROW_ARRAY_H
