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

#ifndef LUMAGL_WEBGPU_DATA_WEBGPU_SCHEMA_H
#define LUMAGL_WEBGPU_DATA_WEBGPU_SCHEMA_H

#include <algorithm>
#include <memory>
#include <string>
#include <vector>

#include "./webgpu-field.h"

namespace lumagl {

/// \brief Sequence of WebGPUField objects describing the columns of a table data structure.
class WebGPUSchema {
 public:
  explicit WebGPUSchema(const std::vector<std::shared_ptr<WebGPUField>>& fields) : _fields{fields} {}

  /// \brief Returns number of fields that this schema contains.
  auto numFields() const -> int { return static_cast<int>(this->_fields.size()); }

  /// Returns the ith schema element. Does not boundscheck.
  auto field(int i) const -> std::shared_ptr<WebGPUField> { return this->_fields[i]; };

  /// \brief Returns a collection of fields that are part of this schema.
  auto fields() const -> const std::vector<std::shared_ptr<WebGPUField>>& { return this->_fields; };

  // TODO(ilija@unfolded.ai): Use a map if we actually end up using these often

  /// \brief Returns a collection of field names that are part of this schema.
  auto fieldNames() const -> std::vector<std::string>;

  /// \brief Attempts to get a field by its name, returns nullptr if it could not be found.
  auto getFieldByName(const std::string& name) const -> std::shared_ptr<WebGPUField>;

 private:
  std::vector<std::shared_ptr<WebGPUField>> _fields;
};

}  // namespace lumagl

#endif  // LUMAGL_WEBGPU_DATA_WEBGPU_SCHEMA_H
