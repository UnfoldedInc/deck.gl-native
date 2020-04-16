// Copyright (c) 2020 Unfolded, Inc.
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

#ifndef LUMAGL_WEGPU_ATTRIBUTE_DESCRIPTOR_H
#define LUMAGL_WEGPU_ATTRIBUTE_DESCRIPTOR_H

#include <dawn/webgpu_cpp.h>

#include <string>
#include <utility>

#include "luma.gl/webgpu/src/webgpu-utils.h"

namespace lumagl {

struct AttributeDescriptor {
 public:
  // TODO(ilija@unfolded.ai): Remove
  AttributeDescriptor() {}
  AttributeDescriptor(const std::string& name, const wgpu::VertexFormat format)
      : name{std::move(name)}, format{format} {}

  std::string name;
  wgpu::VertexFormat format;

  /// \brief Type size in bytes.
  auto size() -> size_t { return lumagl::utils::getVertexFormatSize(this->format); }
};

}  // namespace lumagl

#endif  // LUMAGL_WEGPU_ATTRIBUTE_DESCRIPTOR_H
