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

#include "math.gl/core.h"

namespace deckgl {

class Row {
 public:
  Row(const std::shared_ptr<arrow::Table>& table, int64_t rowIndex);

  auto getInt(const std::string& columnName, int defaultValue = 0) const -> int;
  auto getFloat(const std::string& columnName, float defaultValue = 0.0) const -> float;
  auto getDouble(const std::string& columnName, double defaultValue = 0.0) const -> double;
  auto getBool(const std::string& columnName, bool defaultValue = false) const -> bool;
  auto getString(const std::string& columnName, const std::string& defaultValue = "") const -> std::string;

  auto getFloatVector2(const std::string& columnName, const mathgl::Vector2<float>& defaultValue = {}) const
      -> mathgl::Vector2<float>;
  auto getDoubleVector2(const std::string& columnName, const mathgl::Vector2<double>& defaultValue = {}) const
      -> mathgl::Vector2<double>;
  auto getFloatVector3(const std::string& columnName, const mathgl::Vector3<float>& defaultValue = {}) const
      -> mathgl::Vector3<float>;
  auto getDoubleVector3(const std::string& columnName, const mathgl::Vector3<double>& defaultValue = {}) const
      -> mathgl::Vector3<double>;
  auto getFloatVector4(const std::string& columnName, const mathgl::Vector4<float>& defaultValue = {}) const
      -> mathgl::Vector4<float>;
  auto getDoubleVector4(const std::string& columnName, const mathgl::Vector4<double>& defaultValue = {}) const
      -> mathgl::Vector4<double>;

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
  auto _vector2FromFloatArray(const std::shared_ptr<arrow::FloatArray>& values, int32_t offset, int32_t listSize,
                              const mathgl::Vector2<float>& defaultValue) const -> mathgl::Vector2<float>;
  auto _vector2FromDoubleArray(const std::shared_ptr<arrow::DoubleArray>& values, int32_t offset, int32_t listSize,
                               const mathgl::Vector2<double>& defaultValue) const -> mathgl::Vector2<double>;
  auto _vector3FromFloatArray(const std::shared_ptr<arrow::FloatArray>& values, int32_t offset, int32_t listSize,
                              const mathgl::Vector3<float>& defaultValue) const -> mathgl::Vector3<float>;
  auto _vector3FromDoubleArray(const std::shared_ptr<arrow::DoubleArray>& values, int32_t offset, int32_t listSize,
                               const mathgl::Vector3<double>& defaultValue) const -> mathgl::Vector3<double>;
  auto _vector4FromFloatArray(const std::shared_ptr<arrow::FloatArray>& values, int32_t offset, int32_t listSize,
                              const mathgl::Vector4<float>& defaultValue) const -> mathgl::Vector4<float>;
  auto _vector4FromDoubleArray(const std::shared_ptr<arrow::DoubleArray>& values, int32_t offset, int32_t listSize,
                               const mathgl::Vector4<double>& defaultValue) const -> mathgl::Vector4<double>;

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
