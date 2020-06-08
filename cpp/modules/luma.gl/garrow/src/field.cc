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

#include "./schema.h"  // NOLINT(build/include)

using namespace lumagl::garrow;

Field::Field(const std::string& name, wgpu::VertexFormat type, bool nullable,
             const std::shared_ptr<const KeyValueMetadata>& metadata)
    : _name{name}, _type{type}, _nullable{nullable}, _metadata{metadata} {
  if (nullable) {
    throw std::runtime_error("Nullable fields currently not supported");
  }
}

auto Field::Equals(const Field& other, bool check_metadata) const -> bool {
  if (this == &other) {
    return true;
  }

  if (this->_name == other._name && this->_nullable == other._nullable && this->_type == other._type) {
    if (!check_metadata) {
      return true;
    } else if (this->HasMetadata() && other.HasMetadata()) {
      return this->_metadata->Equals(*other._metadata);
    } else if (!this->HasMetadata() && !other.HasMetadata()) {
      return true;
    } else {
      return false;
    }
  }

  return false;
}

auto Field::Equals(const std::shared_ptr<Field>& other, bool check_metadata) const -> bool {
  return Equals(*other.get(), check_metadata);
}
