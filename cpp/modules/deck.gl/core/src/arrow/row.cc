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

#include "./row.h"  // NOLINT(build/include)

#include <arrow/array.h>

using namespace deckgl;
using namespace mathgl;

Row::Row(const std::shared_ptr<arrow::Table>& table, int rowIndex) : _table{table}, _rowIndex{rowIndex} {
  this->_chunkRowIndex = this->_getChunkRowIndex(table, rowIndex);
}

// NOTE: Accessors largely based on https://arrow.apache.org/docs/cpp/examples/row_columnar_conversion.html

auto Row::getInt(const std::string& columnName, int defaultValue) -> int {
  if (!this->isValid(columnName)) {
    return defaultValue;
  }

  auto chunk = this->_getChunk(columnName);
  auto doubleValue = this->_getDouble(chunk);
  if (doubleValue.has_value()) {
    return static_cast<int>(doubleValue.value());
  }

  return defaultValue;
}

auto Row::getFloat(const std::string& columnName, float defaultValue) -> float {
  if (!this->isValid(columnName)) {
    return defaultValue;
  }

  auto chunk = this->_getChunk(columnName);
  auto doubleValue = this->_getDouble(chunk);
  if (doubleValue.has_value()) {
    return static_cast<float>(doubleValue.value());
  }

  return defaultValue;
}

auto Row::getDouble(const std::string& columnName, double defaultValue) -> double {
  if (!this->isValid(columnName)) {
    return defaultValue;
  }

  auto chunk = this->_getChunk(columnName);
  auto doubleValue = this->_getDouble(chunk);
  if (doubleValue.has_value()) {
    return doubleValue.value();
  }

  return defaultValue;
}

auto Row::getBool(const std::string& columnName, bool defaultValue) -> bool {
  if (!this->isValid(columnName)) {
    return defaultValue;
  }

  auto chunk = this->_getChunk(columnName);
  if (auto boolArray = std::dynamic_pointer_cast<arrow::BooleanArray>(chunk)) {
    return boolArray->Value(this->_chunkRowIndex);
  } else if (auto doubleValue = this->_getDouble(chunk)) {
    return static_cast<bool>(doubleValue.value());
  }

  return defaultValue;
}

auto Row::getString(const std::string& columnName, const std::string& defaultValue) -> std::string {
  if (!this->isValid(columnName)) {
    return defaultValue;
  }

  auto chunk = this->_getChunk(columnName);
  if (auto stringArray = std::dynamic_pointer_cast<arrow::StringArray>(chunk)) {
    return stringArray->GetString(this->_chunkRowIndex);
  }

  return defaultValue;
}

auto Row::getFloatVector2(const std::string& columnName, const Vector2<float>& defaultValue) -> Vector2<float> {
  if (!this->isValid(columnName)) {
    return defaultValue;
  }

  auto chunk = this->_getChunk(columnName);
  switch (chunk->type_id()) {
    case arrow::Type::FIXED_SIZE_LIST: {
      auto listArray = std::static_pointer_cast<arrow::FixedSizeListArray>(chunk);
      auto offset = listArray->value_offset(this->_chunkRowIndex);

      auto values = std::static_pointer_cast<arrow::FloatArray>(listArray->values());
      return this->_vector2FromFloatArray(values, offset, listArray->value_length(), defaultValue);
    }
    case arrow::Type::LIST: {
      auto listArray = std::static_pointer_cast<arrow::ListArray>(chunk);
      auto offset = listArray->value_offset(this->_chunkRowIndex);
      auto length = listArray->value_length(this->_chunkRowIndex);

      auto values = std::static_pointer_cast<arrow::FloatArray>(listArray->values());
      return this->_vector2FromFloatArray(values, offset, length, defaultValue);
    }
    default:
      return defaultValue;
  }
}

auto Row::getDoubleVector2(const std::string& columnName, const Vector2<double>& defaultValue) -> Vector2<double> {
  if (!this->isValid(columnName)) {
    return defaultValue;
  }

  auto chunk = this->_getChunk(columnName);
  switch (chunk->type_id()) {
    case arrow::Type::FIXED_SIZE_LIST: {
      auto listArray = std::static_pointer_cast<arrow::FixedSizeListArray>(chunk);
      auto offset = listArray->value_offset(this->_chunkRowIndex);

      auto values = std::static_pointer_cast<arrow::DoubleArray>(listArray->values());
      return this->_vector2FromDoubleArray(values, offset, listArray->value_length(), defaultValue);
    }
    case arrow::Type::LIST: {
      auto listArray = std::static_pointer_cast<arrow::ListArray>(chunk);
      auto offset = listArray->value_offset(this->_chunkRowIndex);
      auto length = listArray->value_length(this->_chunkRowIndex);

      auto values = std::static_pointer_cast<arrow::DoubleArray>(listArray->values());
      return this->_vector2FromDoubleArray(values, offset, length, defaultValue);
    }
    default:
      return defaultValue;
  }
}

auto Row::getFloatVector3(const std::string& columnName, const Vector3<float>& defaultValue) -> Vector3<float> {
  if (!this->isValid(columnName)) {
    return defaultValue;
  }

  auto chunk = this->_getChunk(columnName);
  switch (chunk->type_id()) {
    case arrow::Type::FIXED_SIZE_LIST: {
      auto listArray = std::static_pointer_cast<arrow::FixedSizeListArray>(chunk);
      auto offset = listArray->value_offset(this->_chunkRowIndex);

      auto values = std::static_pointer_cast<arrow::FloatArray>(listArray->values());
      return this->_vector3FromFloatArray(values, offset, listArray->value_length(), defaultValue);
    }
    case arrow::Type::LIST: {
      auto listArray = std::static_pointer_cast<arrow::ListArray>(chunk);
      auto offset = listArray->value_offset(this->_chunkRowIndex);
      auto length = listArray->value_length(this->_chunkRowIndex);

      auto values = std::static_pointer_cast<arrow::FloatArray>(listArray->values());
      return this->_vector3FromFloatArray(values, offset, length, defaultValue);
    }
    default:
      return defaultValue;
  }
}

auto Row::getDoubleVector3(const std::string& columnName, const Vector3<double>& defaultValue) -> Vector3<double> {
  if (!this->isValid(columnName)) {
    return defaultValue;
  }

  auto chunk = this->_getChunk(columnName);
  switch (chunk->type_id()) {
    case arrow::Type::FIXED_SIZE_LIST: {
      auto listArray = std::static_pointer_cast<arrow::FixedSizeListArray>(chunk);
      auto offset = listArray->value_offset(this->_chunkRowIndex);

      auto values = std::static_pointer_cast<arrow::DoubleArray>(listArray->values());
      return this->_vector3FromDoubleArray(values, offset, listArray->value_length(), defaultValue);
    }
    case arrow::Type::LIST: {
      auto listArray = std::static_pointer_cast<arrow::ListArray>(chunk);
      auto offset = listArray->value_offset(this->_chunkRowIndex);
      auto length = listArray->value_length(this->_chunkRowIndex);

      auto values = std::static_pointer_cast<arrow::DoubleArray>(listArray->values());
      return this->_vector3FromDoubleArray(values, offset, length, defaultValue);
    }
    default:
      return defaultValue;
  }
}

auto Row::isValid(const std::string& columnName) -> bool {
  try {
    auto chunk = this->_getChunk(columnName);
    return chunk->IsValid(this->_rowIndex);
  } catch (...) {
    return false;
  }
}

auto Row::_getChunk(const std::string& columnName) -> std::shared_ptr<arrow::Array> {
  auto column = this->_table->GetColumnByName(columnName);
  if (column == nullptr) {
    throw std::logic_error("Invalid column name");
  }

  // Iterate over column chunks and find the chunk that contains row data
  int64_t offset = 0;
  for (auto chunk : column->chunks()) {
    if (_rowIndex >= offset && _rowIndex < offset + chunk->length()) {
      // TODO(ilija): Cache
      return chunk;
    }

    offset += chunk->length();
  }

  throw std::logic_error("Invalid chunk lookup");
}

auto Row::_getChunkRowIndex(const std::shared_ptr<arrow::Table>& table, int rowIndex) -> int {
  if (rowIndex >= table->num_rows()) {
    throw std::range_error("Invalid row index");
  }

  // NOTE: Columns are chunked in the same way, so we pick an arbitrary column
  // https://arrow.apache.org/docs/cpp/tables.html#tables
  auto column = table->column(0);

  // Iterate over column chunks and find the chunk that contains row data
  int chunkIndex = rowIndex;
  for (auto chunk : column->chunks()) {
    if (chunkIndex < chunk->length()) {
      return chunkIndex;
    }

    chunkIndex -= chunk->length();
  }

  throw std::logic_error("Invalid chunk lookup");
}

// TODO(ilija): Sacrificing performance and precision for conciseness, revisit
auto Row::_getDouble(const std::shared_ptr<arrow::Array>& chunk) -> std::optional<double> {
  switch (chunk->type_id()) {
    case arrow::Type::DOUBLE:
      return std::static_pointer_cast<arrow::DoubleArray>(chunk)->Value(this->_chunkRowIndex);
    case arrow::Type::FLOAT:
      return static_cast<double>(std::static_pointer_cast<arrow::FloatArray>(chunk)->Value(this->_chunkRowIndex));
    case arrow::Type::INT64:
      return static_cast<double>(std::static_pointer_cast<arrow::Int64Array>(chunk)->Value(this->_chunkRowIndex));
    case arrow::Type::INT32:
      return static_cast<double>(std::static_pointer_cast<arrow::Int32Array>(chunk)->Value(this->_chunkRowIndex));

    default:
      return std::nullopt;
  }
}

auto Row::_vector2FromFloatArray(const std::shared_ptr<arrow::FloatArray>& values, int32_t offset, int32_t listSize,
                                 const Vector2<float>& defaultValue) -> Vector2<float> {
  if (values->type_id() != arrow::Type::FLOAT) { return defaultValue; }
  if (listSize < 1) { return defaultValue; }

  // Needed for some reason as seen in https://arrow.apache.org/docs/cpp/examples/row_columnar_conversion.html
  auto listPointer = values->data()->GetValues<float>(1);
  const float first = *(listPointer + offset);
  const float second = listSize >= 2 ? *(listPointer + offset + 1) : 0.0;

  return Vector2<float>(first, second);
}

auto Row::_vector2FromDoubleArray(const std::shared_ptr<arrow::DoubleArray>& values, int32_t offset, int32_t listSize,
                                  const Vector2<double>& defaultValue) -> Vector2<double> {
  if (values->type_id() != arrow::Type::DOUBLE) { return defaultValue; }
  if (listSize < 1) { return defaultValue; }

  // Needed for some reason as seen in https://arrow.apache.org/docs/cpp/examples/row_columnar_conversion.html
  auto listPointer = values->data()->GetValues<double>(1);
  const double first = *(listPointer + offset);
  const double second = listSize >= 2 ? *(listPointer + offset + 1) : 0.0;

  return Vector2<double>(first, second);
}

auto Row::_vector3FromFloatArray(const std::shared_ptr<arrow::FloatArray>& values, int32_t offset, int32_t listSize,
                                 const Vector3<float>& defaultValue) -> Vector3<float> {
  if (values->type_id() != arrow::Type::FLOAT) { return defaultValue; }
  if (listSize < 2) { return defaultValue; }

  // Needed for some reason as seen in https://arrow.apache.org/docs/cpp/examples/row_columnar_conversion.html
  auto listPointer = values->data()->GetValues<float>(1);
  const float first = *(listPointer + offset);
  const float second = *(listPointer + offset + 1);
  const float third = listSize >= 3 ? *(listPointer + offset + 2) : 0.0;

  return Vector3<float>(first, second, third);
}

auto Row::_vector3FromDoubleArray(const std::shared_ptr<arrow::DoubleArray>& values, int32_t offset, int32_t listSize,
                                  const Vector3<double>& defaultValue) -> Vector3<double> {
  if (values->type_id() != arrow::Type::DOUBLE) { return defaultValue; }
  if (listSize < 2) { return defaultValue; }

  // Needed for some reason as seen in https://arrow.apache.org/docs/cpp/examples/row_columnar_conversion.html
  auto listPointer = values->data()->GetValues<double>(1);
  const double first = *(listPointer + offset);
  const double second = *(listPointer + offset + 1);
  const double third = listSize >= 3 ? *(listPointer + offset + 2) : 0.0;

  return Vector3<double>(first, second, third);
}
