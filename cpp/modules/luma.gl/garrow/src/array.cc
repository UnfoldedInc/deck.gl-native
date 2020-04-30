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

#include "./array.h"  // NOLINT(build/include)

#include "./util/arrow-utils.h"
#include "./util/webgpu-utils.h"

using namespace lumagl::garrow;

Array::Array(wgpu::Device device, const std::shared_ptr<arrow::Array>& data, wgpu::BufferUsage usage) : Array{device} {
  this->setData(data, usage);
}

Array::~Array() {
  if (this->_buffer != nullptr) {
    this->_buffer.Destroy();
  }
}

void Array::setData(const std::shared_ptr<arrow::Array>& data, wgpu::BufferUsage usage) {
  auto vertexFormatOptional = vertexFormatFromArrowType(data->type());
  if (!vertexFormatOptional.has_value()) {
    throw std::runtime_error("Unsupported data format");
  }
  auto vertexFormat = vertexFormatOptional.value();
  auto vertexSize = getVertexFormatSize(vertexFormat);

  if (!this->_buffer || data->length() != this->_length) {
    auto byteLength = vertexSize * data->length();
    this->_buffer = this->_createBuffer(this->_device, byteLength, usage);
  }

  // TODO(ilija@unfolded.ai): Handle arrays with null values correctly
  if (data->null_count() > 0) {
    throw new std::runtime_error("Data with null values is currently not supported");
  }

  // If child_data isn't empty, data is a list array
  if (!data->data()->child_data.empty()) {
    uint64_t offset = 0;

    // Go through child data for list data types and use child buffers
    for (auto const& childData : data->data()->child_data) {
      // We assume this is a NumericArray, no easy way to check if that's the case because it's templated
      auto dataBuffer = childData->buffers[1];

      this->_buffer.SetSubData(offset, dataBuffer->size(), dataBuffer->data());
      offset += dataBuffer->size();
    }
  } else {
    // Primite data type, just iterate over the buffers
    // We assume this is a NumericArray, no easy way to check if that's the case because it's templated
    auto dataBuffer = data->data()->buffers[1];
    this->_buffer.SetSubData(0, dataBuffer->size(), dataBuffer->data());
  }

  this->_length = data->length();
}

auto Array::_createBuffer(wgpu::Device device, uint64_t size, wgpu::BufferUsage usage) -> wgpu::Buffer {
  wgpu::BufferDescriptor bufferDesc;
  bufferDesc.size = size;
  bufferDesc.usage = wgpu::BufferUsage::CopyDst | usage;

  return device.CreateBuffer(&bufferDesc);
}
