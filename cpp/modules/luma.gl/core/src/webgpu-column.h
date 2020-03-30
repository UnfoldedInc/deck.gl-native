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

namespace lumagl {

#include <dawn.h>  // NOLINT(build/include)

class WebGPUColumn {
 public:
  wgpu::BindGroup bindGroup;
  wgpu::Buffer ubo;

  explicit WebGPUColumn(void *device) {
    wgpu::BufferDescriptor bufferDesc;
    bufferDesc.size = kNumTriangles * sizeof(ShaderData);
    bufferDesc.usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Uniform;
    this->ubo = device.CreateBuffer(&bufferDesc);

    this->bindGroup =
        utils::MakeBindGroup(device, bgl, {{0, this->ubo, 0, this}});
  }

  auto length_in_bytes() => int { return this->lengthInBytes; }
}

}  // namespace lumagl

#endif  // LUMAGL_CORE_WEBGPU_COLUMN_H
