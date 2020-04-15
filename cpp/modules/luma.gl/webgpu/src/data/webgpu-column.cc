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

#include "./webgpu-column.h"  // NOLINT(build/include)

using namespace lumagl;

WebGPUColumn::WebGPUColumn(wgpu::Device device, const std::shared_ptr<AttributeDescriptor>& descriptor) {
  this->_device = device;
  this->_descriptor = descriptor;
}

WebGPUColumn::~WebGPUColumn() { this->buffer.Destroy(); }

void WebGPUColumn::setData(const std::shared_ptr<arrow::Array>& data) {
  if (!this->buffer || data->length() != this->length) {
    auto size = this->_descriptor->typeSize * data->length();
    this->buffer = this->_createBuffer(this->_device, size, wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Vertex);
  }

  // TODO(ilija@unfolded.ai): Handle arrays with null values correctly
  if (data->null_count() > 0) {
    throw new std::runtime_error("Data with null values is currently not supported");
  }

  auto buffers = data->data()->buffers;
  uint64_t offset = 0;
  // Starting from buffer at index 1, as the first buffer is used for null bitmap
  for (int i = 1; i < buffers.size(); i++) {
    auto buffer = buffers[i];

    this->buffer.SetSubData(offset, buffer->size(), buffer->data());
    offset += buffer->size();
  }
}

auto WebGPUColumn::_createBuffer(wgpu::Device device, uint64_t size, wgpu::BufferUsage usage) -> wgpu::Buffer {
  wgpu::BufferDescriptor bufferDesc;
  bufferDesc.size = size;
  bufferDesc.usage = usage;

  return device.CreateBuffer(&bufferDesc);
}
