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

#ifndef DECKGL_CORE_ARROW_ROW_H
#define DECKGL_CORE_ARROW_ROW_H

#include <arrow/array.h>
#include <arrow/table.h>

#include <memory>
#include <optional>
#include <string>
#include <tuple>
#include <vector>

#include "math.gl/core.h"

namespace deckgl {

struct ListArrayMetadata {
 public:
  ListArrayMetadata(int32_t offset, int32_t length, const std::shared_ptr<arrow::Array>& values)
      : offset{offset}, length{length}, values{values} {}

  int32_t offset;
  int32_t length;
  std::shared_ptr<arrow::Array> values;
};

class Row {
 public:
  Row(const std::shared_ptr<arrow::Table>& table, int64_t rowIndex);

  auto getInt(const std::string& columnName, int defaultValue = 0) const -> int;
  auto getFloat(const std::string& columnName, float defaultValue = 0.0) const -> float;
  auto getDouble(const std::string& columnName, double defaultValue = 0.0) const -> double;
  auto getBool(const std::string& columnName, bool defaultValue = false) const -> bool;
  auto getString(const std::string& columnName, const std::string& defaultValue = "") const -> std::string;

  template <typename T>
  auto getVector2(const std::string& columnName, const mathgl::Vector2<T>& defaultValue = {}) const
      -> mathgl::Vector2<T> {
    auto data = this->_getVectorData<T>(columnName);
    if (!data) {
      return defaultValue;
    }

    return mathgl::Vector2<T>{data->size() > 0 ? data->at(0) : 0, data->size() > 1 ? data->at(1) : 0};
  }
  template <typename T>
  auto getVector3(const std::string& columnName, const mathgl::Vector3<T>& defaultValue = {}) const
      -> mathgl::Vector3<T> {
    auto data = this->_getVectorData<T>(columnName);
    if (!data) {
      return defaultValue;
    }

    return mathgl::Vector3<T>{data->size() > 0 ? data->at(0) : 0, data->size() > 1 ? data->at(1) : 0,
                              data->size() > 2 ? data->at(2) : 0};
  }
  template <typename T>
  auto getVector4(const std::string& columnName, const mathgl::Vector4<T>& defaultValue = {}) const
      -> mathgl::Vector4<T> {
    auto data = this->_getVectorData<T>(columnName);
    if (!data) {
      return defaultValue;
    }

    return mathgl::Vector4<T>{data->size() > 0 ? data->at(0) : 0, data->size() > 1 ? data->at(1) : 0,
                              data->size() > 2 ? data->at(2) : 0, data->size() > 3 ? data->at(3) : 0};
  }

  /// \brief Checks whether value at this row, for columnName is valid and not null.
  /// \param columnName Name of the column who's value to check.
  /// \return true if the value for columnName is valid and not null, false otherwise.
  auto isValid(const std::string& columnName) const -> bool;

  /// \brief Increments current row index.
  void incrementRowIndex(uint64_t increment = 1);

 private:
  /// \brief Retrieves a chunk for a given columName.
  /// \param columnName Name of the column to do the lookup for.
  /// \return Returns a chunk within column named columnName, that this row belongs to.
  /// \throw Throws an exception if columnName does not exist in the table, or if the chunk couldn't be found.
  auto _getChunk(const std::string& columnName) const -> std::shared_ptr<arrow::Array>;

  /// \brief Calculates a relative index of a row within the appropriate chunk.
  /// \param table Table to do chunk lookup in.
  /// \param rowIndex Index of the row to do lookup for.
  /// \return Tuple containing index of the chunk this row belongs to, and a row index within the chunk.
  /// \throw Throws an exception if rowIndex exceeds table bounds, or if the chunk couldn't be found.
  auto _getRowChunkData(const std::shared_ptr<arrow::Table>& table, int64_t rowIndex) const -> std::tuple<int, int64_t>;

  auto _getDouble(const std::shared_ptr<arrow::Array>& chunk) const -> std::optional<double>;
  auto _getListArrayMetadata(const std::shared_ptr<arrow::Array>& array, int64_t index) const
      -> std::shared_ptr<ListArrayMetadata>;

  template <typename T>
  auto _getVectorData(const std::string& columnName) const -> std::shared_ptr<std::vector<T>> {
    if (!this->isValid(columnName)) {
      return nullptr;
    }

    auto metadata = this->_getListArrayMetadata(this->_getChunk(columnName), this->_chunkRowIndex);

    auto data = std::make_shared<std::vector<T>>();
    if (auto doubleArray = std::dynamic_pointer_cast<arrow::DoubleArray>(metadata->values)) {
      for (int i = 0; i < metadata->length; i++) {
        data->push_back(static_cast<T>(doubleArray->Value(metadata->offset + i)));
      }
    } else if (auto floatArray = std::dynamic_pointer_cast<arrow::FloatArray>(metadata->values)) {
      for (int i = 0; i < metadata->length; i++) {
        data->push_back(static_cast<T>(floatArray->Value(metadata->offset + i)));
      }
    } else if (auto intArray = std::dynamic_pointer_cast<arrow::Int64Array>(metadata->values)) {
      for (int i = 0; i < metadata->length; i++) {
        data->push_back(static_cast<T>(intArray->Value(metadata->offset + i)));
      }
    } else if (auto intArray = std::dynamic_pointer_cast<arrow::Int32Array>(metadata->values)) {
      for (int i = 0; i < metadata->length; i++) {
        data->push_back(static_cast<T>(intArray->Value(metadata->offset + i)));
      }
    } else {
      return nullptr;
    }

    return data;
  }

  /// \brief Table that this row belongs to.
  std::shared_ptr<arrow::Table> _table;

  /// \brief Index of the row within table.
  int64_t _rowIndex;

  /// \brief Index of the chunk that this row belongs to.
  int _chunkIndex;

  /// \brief Relative row index in respect to the chunk.
  int64_t _chunkRowIndex;

  //  std::unordered_map<std::string, std::shared_ptr<arrow::Array>> _cache;
};

}  // namespace deckgl

#endif  // DECKGL_CORE_ARROW_ROW_H
