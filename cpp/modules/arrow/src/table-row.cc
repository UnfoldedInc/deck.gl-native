#include "table-row.h"

#include <arrow/array.h>

using namespace deckgl;

TableRow::TableRow(const std::shared_ptr<arrow::Table>& table, int rowIndex) : _table{table}, _rowIndex{rowIndex} {
  if (rowIndex >= table->num_rows()) {
    throw std::range_error("Invalid row index");
  }
  _chunkIndex = _getChunkIndex(table, rowIndex);
}

auto TableRow::getString(const std::string& columnName) -> std::string {
  auto chunk = this->_getChunk(columnName);
  if (auto stringArray = std::dynamic_pointer_cast<arrow::StringArray>(chunk)) {
    return stringArray->GetString(_chunkIndex);
  }

  throw std::logic_error("Invalid column type: " + chunk->type()->ToString());
}

auto TableRow::_getChunk(const std::string& columnName) -> std::shared_ptr<arrow::Array> {
  auto column = _table->GetColumnByName(columnName);
  if (column == nullptr) {
    throw std::logic_error("Invalid column name");
  }

  // Iterate over column chunks and find the chunk that contains row data
  int64_t offset = 0;
  for (auto chunk : column->chunks()) {
    if (_rowIndex >= offset && _rowIndex < offset + chunk->length()) {
      // TODO: Cache
      return chunk;
    }

    offset += chunk->length();
  }

  throw std::logic_error("Invalid chunk lookup");
}

auto TableRow::_getChunkIndex(const std::shared_ptr<arrow::Table>& table, int rowIndex) -> int {
  // Return -1 for an invalid lookup
  if (table->num_columns() == 0 || rowIndex >= table->num_rows()) {
    return -1;
  }

  // NOTE: Columns are chunked in the same way, so we pick an arbitrary column
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
