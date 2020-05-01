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

Row::Row(const std::shared_ptr<arrow::Table>& table, int64_t rowIndex) : _table{table}, _rowIndex{rowIndex} {
  auto [chunkIndex, chunkRowIndex] = this->_getRowChunkData(table, rowIndex);

  this->_chunkIndex = chunkIndex;
  this->_chunkRowIndex = chunkRowIndex;
}

// NOTE: Accessors largely based on https://arrow.apache.org/docs/cpp/examples/row_columnar_conversion.html

auto Row::getInt(const std::string& columnName, int defaultValue) const -> int {
  if (!this->isValid(columnName)) {
    return defaultValue;
  }

  auto chunk = this->_getChunk(columnName);
  auto doubleValue = this->_getDouble(chunk);
  if (doubleValue) {
    return static_cast<int>(doubleValue.value());
  }

  return defaultValue;
}

auto Row::getFloat(const std::string& columnName, float defaultValue) const -> float {
  if (!this->isValid(columnName)) {
    return defaultValue;
  }

  auto chunk = this->_getChunk(columnName);
  auto doubleValue = this->_getDouble(chunk);
  if (doubleValue) {
    return static_cast<float>(doubleValue.value());
  }

  return defaultValue;
}

auto Row::getDouble(const std::string& columnName, double defaultValue) const -> double {
  if (!this->isValid(columnName)) {
    return defaultValue;
  }

  auto chunk = this->_getChunk(columnName);
  auto doubleValue = this->_getDouble(chunk);
  if (doubleValue) {
    return doubleValue.value();
  }

  return defaultValue;
}

auto Row::getBool(const std::string& columnName, bool defaultValue) const -> bool {
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

auto Row::getString(const std::string& columnName, const std::string& defaultValue) const -> std::string {
  if (!this->isValid(columnName)) {
    return defaultValue;
  }

  auto chunk = this->_getChunk(columnName);
  if (auto stringArray = std::dynamic_pointer_cast<arrow::StringArray>(chunk)) {
    return stringArray->GetString(this->_chunkRowIndex);
  }

  return defaultValue;
}

auto Row::isValid(const std::string& columnName) const -> bool {
  try {
    auto chunk = this->_getChunk(columnName);
    return chunk->IsValid(this->_rowIndex);
  } catch (...) {
    return false;
  }
}

void Row::incrementRowIndex(uint64_t increment) {
  int64_t newRowIndex = this->_rowIndex + increment;
  if (newRowIndex >= this->_table->num_rows()) {
    throw std::range_error("Increment index out of bounds");
  }

  // NOTE: Columns are chunked in the same way, so we pick an arbitrary column
  // https://arrow.apache.org/docs/cpp/tables.html#tables
  auto column = this->_table->column(0);

  // Look up incremented row, starting with the chunk previous row index was in
  int chunkIndex = this->_chunkIndex;
  auto chunk = column->chunk(chunkIndex);
  int64_t newChunkRowIndex = this->_chunkRowIndex + increment;
  int64_t stepSize = chunk->length() - this->_chunkRowIndex;
  while (newChunkRowIndex >= chunk->length()) {
    newChunkRowIndex -= stepSize;
    chunk = column->chunk(++chunkIndex);
    stepSize = chunk->length();
  }

  this->_chunkIndex = chunkIndex;
  this->_chunkRowIndex = newChunkRowIndex;
  this->_rowIndex = newRowIndex;
}

auto Row::_getChunk(const std::string& columnName) const -> std::shared_ptr<arrow::Array> {
  auto column = this->_table->GetColumnByName(columnName);
  if (column == nullptr) {
    throw std::logic_error("Invalid column name");
  }

  // Iterate over column chunks and find the chunk that contains row data
  int64_t offset = 0;
  for (auto chunk : column->chunks()) {
    if (_rowIndex >= offset && _rowIndex < offset + chunk->length()) {
      // TODO(ilija@unfolded.ai): Cache
      return chunk;
    }

    offset += chunk->length();
  }

  throw std::logic_error("Invalid chunk lookup");
}

auto Row::_getRowChunkData(const std::shared_ptr<arrow::Table>& table, int64_t rowIndex) const
    -> std::tuple<int, int64_t> {
  if (rowIndex >= table->num_rows()) {
    throw std::range_error("Invalid row index");
  }

  // NOTE: Columns are chunked in the same way, so we pick an arbitrary column
  // https://arrow.apache.org/docs/cpp/tables.html#tables
  auto column = table->column(0);

  // Iterate over column chunks and find the chunk that contains row data
  int64_t chunkRowIndex = rowIndex;
  for (int chunkIndex = 0; chunkIndex < column->num_chunks(); ++chunkIndex) {
    auto chunk = column->chunk(chunkIndex);
    if (chunkRowIndex < chunk->length()) {
      return {chunkIndex, chunkRowIndex};
    }

    chunkRowIndex -= chunk->length();
  }

  throw std::logic_error("Invalid chunk lookup");
}

// TODO(ilija@unfolded.ai): Sacrificing performance and precision for conciseness, revisit
auto Row::_getDouble(const std::shared_ptr<arrow::Array>& chunk) const -> std::optional<double> {
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

auto Row::_getListArrayMetadata(const std::shared_ptr<arrow::Array>& array, int64_t index) const
    -> std::shared_ptr<ListArrayMetadata> {
  switch (array->type_id()) {
    case arrow::Type::FIXED_SIZE_LIST: {
      auto listArray = std::static_pointer_cast<arrow::FixedSizeListArray>(array);
      auto offset = listArray->value_offset(index);
      auto length = listArray->value_length();

      return std::make_shared<ListArrayMetadata>(offset, length, listArray->values());
    }
    case arrow::Type::LIST: {
      auto listArray = std::static_pointer_cast<arrow::ListArray>(array);
      auto offset = listArray->value_offset(index);
      auto length = listArray->value_length(index);

      return std::make_shared<ListArrayMetadata>(offset, length, listArray->values());
    }
    default:
      return nullptr;
  }
}
