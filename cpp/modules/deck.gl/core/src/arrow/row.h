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
#include "probe.gl/core.h"

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
    auto data = this->getListData<T>(columnName, {defaultValue.x, defaultValue.y});
    return mathgl::Vector2<T>{data.size() > 0 ? data.at(0) : 0, data.size() > 1 ? data.at(1) : 0};
  }
  template <typename T>
  auto getVector3(const std::string& columnName, const mathgl::Vector3<T>& defaultValue = {}) const
      -> mathgl::Vector3<T> {
    auto data = this->getListData<T>(columnName, {defaultValue.x, defaultValue.y, defaultValue.z});
    return mathgl::Vector3<T>{data.size() > 0 ? data.at(0) : 0, data.size() > 1 ? data.at(1) : 0,
                              data.size() > 2 ? data.at(2) : 0};
  }
  template <typename T>
  auto getVector4(const std::string& columnName, const mathgl::Vector4<T>& defaultValue = {}) const
      -> mathgl::Vector4<T> {
    auto data = this->getListData<T>(columnName, {defaultValue.x, defaultValue.y, defaultValue.z, defaultValue.w});
    return mathgl::Vector4<T>{data.size() > 0 ? data.at(0) : 0, data.size() > 1 ? data.at(1) : 0,
                              data.size() > 2 ? data.at(2) : 0, data.size() > 3 ? data.at(3) : 0};
  }
  template <typename T>
  auto getVector2List(const std::string& columnName, const std::vector<mathgl::Vector2<T>>& defaultValue = {}) const
      -> std::vector<mathgl::Vector2<T>> {
    auto listData = this->getNestedListData<T>(columnName, {});
    std::vector<mathgl::Vector2<T>> vectorData;
    vectorData.reserve(listData.size());
    for (const auto& data : listData) {
      vectorData.push_back(mathgl::Vector2<T>{data.size() > 0 ? data[0] : 0, data.size() > 1 ? data[1] : 0});
    }

    return vectorData;
  }
  template <typename T>
  auto getVector3List(const std::string& columnName, const std::vector<mathgl::Vector3<T>>& defaultValue = {}) const
      -> std::vector<mathgl::Vector3<T>> {
    auto listData = this->getNestedListData<T>(columnName, {});
    std::vector<mathgl::Vector3<T>> vectorData;
    vectorData.reserve(listData.size());
    for (const auto& data : listData) {
      vectorData.push_back(mathgl::Vector3<T>{data.size() > 0 ? data[0] : 0, data.size() > 1 ? data[1] : 0,
                                              data.size() > 2 ? data[2] : 0});
    }

    return vectorData;
  }
  template <typename T>
  auto getVector4List(const std::string& columnName, const std::vector<mathgl::Vector4<T>>& defaultValue = {}) const
      -> std::vector<mathgl::Vector4<T>> {
    auto listData = this->getNestedListData<T>(columnName, {});
    std::vector<mathgl::Vector4<T>> vectorData;
    vectorData.reserve(listData.size());
    for (const auto& data : listData) {
      vectorData.push_back(mathgl::Vector4<T>{data.size() > 0 ? data[0] : 0, data.size() > 1 ? data[1] : 0,
                                              data.size() > 2 ? data[2] : 0, data.size() > 3 ? data[3] : 0});
    }

    return vectorData;
  }
  template <typename T>
  auto getListData(const std::string& columnName, const std::vector<T>& defaultValue = {}) const -> std::vector<T> {
    if (!this->isValid(columnName)) {
      probegl::WarningLog() << "Requested column data not valid, returning default value";
      return defaultValue;
    }

    auto optionalMetadata = this->_getListArrayMetadata(this->_getChunk(columnName), this->_chunkRowIndex);
    if (!optionalMetadata) {
      probegl::WarningLog() << "Requested list type not supported, returning default value";
      return defaultValue;
    }

    return this->_getListData(optionalMetadata.value(), defaultValue);
  }
  template <typename T>
  auto getNestedListData(const std::string& columnName, const std::vector<std::vector<T>>& defaultValue = {}) const
      -> std::vector<std::vector<T>> {
    if (!this->isValid(columnName)) {
      probegl::WarningLog() << "Requested column data not valid, returning default value";
      return defaultValue;
    }

    auto optionalMetadata = this->_getListArrayMetadata(this->_getChunk(columnName), this->_chunkRowIndex);
    if (!optionalMetadata) {
      probegl::WarningLog() << "Requested list type not supported, returning default value";
      return defaultValue;
    }

    return this->_getNestedListData(optionalMetadata.value(), defaultValue);
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
      -> std::optional<ListArrayMetadata>;

  template <typename T>
  auto _getListData(const ListArrayMetadata& metadata, const std::vector<T>& defaultValue = {}) const
      -> std::vector<T> {
    std::vector<T> data;
    switch (metadata.values->type_id()) {
      case arrow::Type::type::DOUBLE: {
        auto doubleArray = std::static_pointer_cast<arrow::DoubleArray>(metadata.values);
        for (int32_t i = 0; i < metadata.length; i++) {
          data.push_back(static_cast<T>(doubleArray->Value(metadata.offset + i)));
        }
        break;
      }
      case arrow::Type::type::FLOAT: {
        auto floatArray = std::static_pointer_cast<arrow::FloatArray>(metadata.values);
        for (int32_t i = 0; i < metadata.length; i++) {
          data.push_back(static_cast<T>(floatArray->Value(metadata.offset + i)));
        }
        break;
      }
      case arrow::Type::type::INT64: {
        auto int64Array = std::static_pointer_cast<arrow::Int64Array>(metadata.values);
        for (int32_t i = 0; i < metadata.length; i++) {
          data.push_back(static_cast<T>(int64Array->Value(metadata.offset + i)));
        }
        break;
      }
      case arrow::Type::type::INT32: {
        auto int32Array = std::static_pointer_cast<arrow::Int32Array>(metadata.values);
        for (int32_t i = 0; i < metadata.length; i++) {
          data.push_back(static_cast<T>(int32Array->Value(metadata.offset + i)));
        }
        break;
      }
      default:
        probegl::WarningLog() << "Unsupported list value type, returning default value";
        return defaultValue;
    }

    return data;
  }

  template <typename T>
  auto _getNestedListData(const ListArrayMetadata& metadata, const std::vector<std::vector<T>>& defaultValue = {}) const
      -> std::vector<std::vector<T>> {
    std::vector<std::vector<T>> data;
    switch (metadata.values->type_id()) {
      case arrow::Type::type::LIST:
      case arrow::Type::type::FIXED_SIZE_LIST: {
        for (int32_t i = 0; i < metadata.length; i++) {
          auto optionalMetadata = this->_getListArrayMetadata(metadata.values, metadata.offset + i);
          if (!optionalMetadata) {
            probegl::WarningLog() << "List type for nested array not supported, returning default value";
            return defaultValue;
          }

          data.push_back(this->_getListData(optionalMetadata.value(), std::vector<T>{}));
        }
        break;
      }
      default:
        probegl::WarningLog() << "Unsupported list value type, returning default value";
        return defaultValue;
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
