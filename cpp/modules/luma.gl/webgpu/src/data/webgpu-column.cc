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

WebGPUColumn::WebGPUColumn(wgpu::Device device, const std::shared_ptr<arrow::Array>& column) {
  // TODO(ilija@unfolded.ai): How do we handle null values, should we provide default values?

  // TODO(ilija@unfolded.ai): Is there a way to get the type size out of column->type()?
  this->elementSize = sizeof(float);

  wgpu::BufferDescriptor bufferDesc;
  bufferDesc.size = column->length() * this->elementSize;
  bufferDesc.usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Uniform;
  this->buffer = device.CreateBuffer(&bufferDesc);

  this->setData(column);

  //  this->bindGroup = utils::makeBindGroup(device, layout, {{0, this->buffer, 0, sizeof(void*)}});
}

WebGPUColumn::~WebGPUColumn() { this->buffer.Destroy(); }

void WebGPUColumn::setData(const std::shared_ptr<arrow::Array>& column) {
  // TODO(ilija@unfolded.ai): Handle null values

  // TODO(ilija@unfolded.ai): Is there a way to get the type size out of column->type()?
  auto typeSize = sizeof(float);

  // TODO(ilija@unfolded.ai): Is there a way to retrieve a continous memory chunk containing all the values,
  // or do we need to iterate over column->data()->buffers, or type cast the array?
  this->buffer.SetSubData(0, column->length() * typeSize, column->data()->buffers[1]->data());
}
