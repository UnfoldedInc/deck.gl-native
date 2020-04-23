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

#ifndef LUMAGL_GARROW_KEY_VALUE_METADATA_H
#define LUMAGL_GARROW_KEY_VALUE_METADATA_H

#include <string>
#include <unordered_map>
#include <vector>

namespace lumagl {
namespace garrow {

/// \brief A container for key-value pair type metadata.
class KeyValueMetadata {
 public:
  KeyValueMetadata();
  KeyValueMetadata(const std::vector<std::string>& keys, const std::vector<std::string>& values);
  explicit KeyValueMetadata(const std::unordered_map<std::string, std::string>& map);

  /// \brief Populates given map with key/value pairs contained in this metadata.
  void ToUnorderedMap(std::unordered_map<std::string, std::string>* out) const;

  /// \brief Appends key and value to metadata.
  /// @param key Key to append.
  /// @param value Value to append.
  void Append(const std::string& key, const std::string& value);

  /// \brief Returns size of key/value pairs contained in the metadata.
  auto size() const -> int64_t { return this->_keys.size(); };

  /// \brief Returns key at given index, does not bound check.
  /// @param i Index of the key to retrieve.
  auto key(int64_t i) const -> const std::string& { return this->_keys[i]; };

  /// \brief Returns value at given index, does not bound check.
  /// @param i Index of the value to retrieve.
  auto value(int64_t i) const -> const std::string& { return this->_values[i]; };

  /// \brief Perform linear search for key, returning -1 if not found.
  /// @param key Key to search for.
  int FindKey(const std::string& key) const;

 private:
  std::vector<std::string> _keys;
  std::vector<std::string> _values;
};

}  // namespace garrow
}  // namespace lumagl

#endif  // LUMAGL_GARROW_KEY_VALUE_METADATA_H
