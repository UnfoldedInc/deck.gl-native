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

namespace deckgl {

class TableRow {
 public:
  TableRow(const std::shared_ptr<arrow::Table>& table, int rowIndex);

  auto getString(const std::string& columnName) -> std::string;

  template <typename ElementType, typename ReturnValue>
  auto getNumber(const std::string& columnName) -> ReturnValue {
    auto chunk = this->_getChunk(columnName);
    if (auto numericArray = std::dynamic_pointer_cast<arrow::NumericArray<ElementType>>(chunk)) {
      return numericArray->Value(_chunkIndex);
    }

    throw std::logic_error("Invalid column type, found: " + chunk->type()->ToString());
  }

  // TODO: Support additional types

 private:
  auto _getChunk(const std::string& columnName) -> std::shared_ptr<arrow::Array>;
  auto _getChunkIndex(const std::shared_ptr<arrow::Table>& table, int rowIndex) -> int;

  std::shared_ptr<arrow::Table> _table;
  int _chunkIndex;
  int _rowIndex;

  //  std::unordered_map<std::string, std::shared_ptr<arrow::Array>> _cache;
};

}  // namespace deckgl

#endif  // DECKGL_CORE_LIB_ATTRIBUTE_TABLE_TABLE_ROW_H
