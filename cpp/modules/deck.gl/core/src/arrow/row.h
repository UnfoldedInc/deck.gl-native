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

#ifndef DECKGL_CORE_LIB_ATTRIBUTE_TABLE_TABLE_ROW_H
#define DECKGL_CORE_LIB_ATTRIBUTE_TABLE_TABLE_ROW_H

#include <arrow/array.h>
#include <arrow/table.h>

#include <memory>
#include <unordered_map>

#include "math.gl/core.h"

using namespace mathgl;

namespace deckgl {

class Row {
 public:
  Row(const std::shared_ptr<arrow::Table>& table, int rowIndex);

  auto getInt(const std::string& columnName, int nullValue = 0) -> int;
  auto getFloat(const std::string& columnName, float nullValue = 0.0) -> float;
  auto getDouble(const std::string& columnName, double nullValue = 0.0) -> double;
  auto getBool(const std::string& columnName, bool nullValue = false) -> bool;
  auto getString(const std::string& columnName, const std::string& nullValue = "") -> std::string;

  auto getFloatVector2(const std::string& columnName, const Vector2<float>& nullValue = {}) -> Vector2<float>;
  auto getDoubleVector2(const std::string& columnName, const Vector2<double>& nullValue = {}) -> Vector2<double>;
  auto getFloatVector3(const std::string& columnName, const Vector3<float>& nullValue = {}) -> Vector3<float>;
  auto getDoubleVector3(const std::string& columnName, const Vector3<double>& nullValue = {}) -> Vector3<double>;

  auto isValid(const std::string& columnName) -> bool;

 private:
  auto _getChunk(const std::string& columnName) -> std::shared_ptr<arrow::Array>;
  auto _getChunkIndex(const std::shared_ptr<arrow::Table>& table, int rowIndex) -> int;

  auto _getDouble(const std::shared_ptr<arrow::Array>& chunk) -> std::optional<double>;
  auto _vector2FromFloatArray(const std::shared_ptr<arrow::FloatArray>& values, const Vector2<float>& nullValue)
      -> Vector2<float>;
  auto _vector2FromDoubleArray(const std::shared_ptr<arrow::DoubleArray>& values, const Vector2<double>& nullValue)
      -> Vector2<double>;
  auto _vector3FromFloatArray(const std::shared_ptr<arrow::FloatArray>& values, const Vector3<float>& nullValue)
      -> Vector3<float>;
  auto _vector3FromDoubleArray(const std::shared_ptr<arrow::DoubleArray>& values, const Vector3<double>& nullValue)
      -> Vector3<double>;

  std::shared_ptr<arrow::Table> _table;
  int _chunkIndex;
  int _rowIndex;

  //  std::unordered_map<std::string, std::shared_ptr<arrow::Array>> _cache;
};

}  // namespace deckgl

#endif  // DECKGL_CORE_LIB_ATTRIBUTE_TABLE_TABLE_ROW_H
