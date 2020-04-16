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

#ifndef LUMAGL_WEBGPU_DATA_WEBGPU_FIELD_H
#define LUMAGL_WEBGPU_DATA_WEBGPU_FIELD_H

#include <memory>
#include <string>
#include <utility>

#include "./attribute-descriptor.h"

namespace lumagl {

/// \brief A combination of a field name and additional metadata specific to this field.
class WebGPUField {
 public:
  // TODO(ilija@unfolded.ai): In order for this to match arrow API, and decouple it from attributes,
  // we should use general-purpoes metadata instead
  WebGPUField(const std::string& name, const std::shared_ptr<AttributeDescriptor>& descriptor)
      : _name{std::move(name)}, _descriptor{descriptor} {}

  /// \brief Returns the field name.
  auto name() const -> const std::string& { return _name; }
  /// \brief Returns the associated descriptor data.
  auto descriptor() const -> std::shared_ptr<AttributeDescriptor> { return _descriptor; }

 private:
  std::string _name;
  std::shared_ptr<AttributeDescriptor> _descriptor;
};

}  // namespace lumagl

#endif  // LUMAGL_WEBGPU_DATA_WEBGPU_FIELD_H