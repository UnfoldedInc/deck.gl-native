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

// TODO(ilija@unfolded.ai): Use a map if we actually end up using these often

auto Schema::field_names() const -> std::vector<std::string> {
  std::vector<std::string> names;
  std::transform(this->_fields.begin(), this->_fields.end(), std::back_inserter(names),
                 [](std::shared_ptr<Field> field) { return field->name(); });
  return names;
};

auto Schema::GetFieldByName(const std::string& name) const -> std::shared_ptr<Field> {
  for (auto const& field : this->_fields) {
    if (field->name() == name) {
      return field;
    }
  }
  return nullptr;
}
