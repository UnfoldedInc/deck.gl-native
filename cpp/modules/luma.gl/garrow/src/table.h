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

#ifndef LUMAGL_GARROW_TABLE_H
#define LUMAGL_GARROW_TABLE_H

#include <memory>
#include <string>
#include <vector>

#include "./array.h"
#include "./schema.h"

namespace lumagl {
namespace garrow {

/// \brief GPU memory table whos columns represent in-memory buffers.
class Table {
 public:
  Table(const std::shared_ptr<Schema>& schema, const std::vector<std::shared_ptr<Array>> arrays)
      : _schema{schema}, _columns{arrays} {}

  /// \brief Returns schema that describes this table.
  auto schema() const -> std::shared_ptr<Schema> { return this->_schema; }

  /// \brief Returns field at index i, does not bound check.
  /// @param i Index of the field to get.
  auto field(int i) const -> std::shared_ptr<Field> { return this->_schema->field(i); }

  /// \brief Returns all the fields in this tables schema.
  auto fields() const -> std::vector<std::shared_ptr<Field>> { return this->_schema->fields(); };

  /// \brief Returns a column at index i, does not bound check.
  /// @param i Index of the column to get.
  auto column(int i) const -> std::shared_ptr<Array> { return this->_columns[i]; };

  /// \brief Returns the columns that this table contains.
  auto columns() const -> std::vector<std::shared_ptr<Array>> { return this->_columns; };

  /// \brief Returns number of columns in this table.
  // TODO(ilija@unfolded.ai): How do we enforce equal row sizes, add padding?
  auto num_columns() const -> int { return this->_schema->num_fields(); }

  /// \brief Return names of all columns
  auto ColumnNames() const -> std::vector<std::string>;

  /// \brief Returns number of rows in this table.
  auto num_rows() const -> int64_t { return this->_columns.empty() ? 0 : this->_columns[0]->length(); }

 private:
  std::shared_ptr<Schema> _schema;
  std::vector<std::shared_ptr<Array>> _columns;
};

}  // namespace garrow
}  // namespace lumagl

#endif  // LUMAGL_GARROW_TABLE_H
