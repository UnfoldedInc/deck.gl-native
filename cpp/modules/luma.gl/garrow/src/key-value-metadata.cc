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

#include "./key-value-metadata.h"  // NOLINT(build/include)

#include <algorithm>

using namespace lumagl::garrow;

KeyValueMetadata::KeyValueMetadata() {}

KeyValueMetadata::KeyValueMetadata(const std::vector<std::string>& keys, const std::vector<std::string>& values)
    : _keys{keys}, _values{values} {
  if (keys.size() != values.size()) {
    throw std::runtime_error("keys and values vectors must have the same size");
  }
}

KeyValueMetadata::KeyValueMetadata(const std::unordered_map<std::string, std::string>& map) {
  std::vector<std::string> keys;
  std::vector<std::string> values;
  for (std::pair<std::string, std::string> element : map) {
    keys.push_back(element.first);
    values.push_back(element.second);
  }

  this->_keys = keys;
  this->_values = values;
}

void KeyValueMetadata::ToUnorderedMap(std::unordered_map<std::string, std::string>* out) const {
  for (int i = 0; i < this->size(); i++) {
    out->insert(std::make_pair(this->_keys[i], this->_values[i]));
  }
}

void KeyValueMetadata::Append(const std::string& key, const std::string& value) {
  this->_keys.push_back(key);
  this->_values.push_back(value);
}

auto KeyValueMetadata::FindKey(const std::string& key) const -> int {
  for (int i = 0; i < this->size(); i++) {
    if (key == this->_keys[i]) {
      return i;
    }
  }

  return -1;
}

auto KeyValueMetadata::Equals(const KeyValueMetadata& other) const -> bool {
  if (size() != other.size()) {
    return false;
  }

  // TODO(ilija@unfolded.ai): Sort these before comparing
  return this->_keys == other._keys && this->_values == other._values;
}
