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

#ifndef LUMAGL_GARROW_FIELD_H
#define LUMAGL_GARROW_FIELD_H

#include <dawn/webgpu_cpp.h>

#include <memory>
#include <string>
#include <utility>

#include "./key-value-metadata.h"

namespace lumagl {
namespace garrow {

/// \brief A combination of a field name and additional metadata specific to this field.
class Field {
 public:
  // NOTE: type is currently a simple wgpu::VertexFormat value. Arrow has a complex DataType implementation that
  // deals with buffer type complexity, which is something that should be put in place once we implement reading
  Field(const std::string& name, wgpu::VertexFormat type, bool nullable = false,
        const std::shared_ptr<const KeyValueMetadata>& metadata = nullptr);

  /// \brief Returns the field name.
  auto name() const -> const std::string& { return _name; }

  /// \brief Return the field data type.
  auto type() const -> wgpu::VertexFormat { return _type; }

  /// \brief Return whether the field is nullable.
  auto nullable() const -> bool { return _nullable; }

  /// \brief Return the field's attached metadata.
  auto metadata() const -> std::shared_ptr<const KeyValueMetadata> { return this->_metadata; }

  /// \brief Return whether the field has non-empty metadata.
  auto HasMetadata() const -> bool { return this->_metadata != nullptr && this->_metadata->size() > 0; };

  /// \brief Indicate if fields are equals.
  ///
  /// \param[in] other field to check equality with.
  /// \param[in] check_metadata controls if it should check for metadata equality.
  ///
  /// \return true if fields are equal, false otherwise.
  auto Equals(const Field& other, bool check_metadata = true) const -> bool;
  auto Equals(const std::shared_ptr<Field>& other, bool check_metadata = true) const -> bool;

 private:
  std::string _name;
  wgpu::VertexFormat _type;
  bool _nullable;
  std::shared_ptr<const KeyValueMetadata> _metadata;
};

}  // namespace garrow
}  // namespace lumagl

#endif  // LUMAGL_GARROW_FIELD_H
