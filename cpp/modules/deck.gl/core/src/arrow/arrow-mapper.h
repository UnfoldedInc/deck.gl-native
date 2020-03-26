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

#ifndef DECKGL_CORE_ARROW_ARROW_TYPES_H
#define DECKGL_CORE_ARROW_ARROW_TYPES_H

#include <arrow/array.h>
#include <arrow/table.h>

#include <memory>

#include "./row.h"
#include "math.gl/core.h"

namespace deckgl {

/// \brief Utility class that provides a way to easily map Arrow tables.
class ArrowMapper {
 public:
  using FloatAccessor = auto(const std::shared_ptr<Row>&) -> float;
  using Vector3FloatAccessor = auto(const std::shared_ptr<Row>&) -> mathgl::Vector3<float>;
  using Vector3DoubleAccessor = auto(const std::shared_ptr<Row>&) -> mathgl::Vector3<double>;
  using Vector4FloatAccessor = auto(const std::shared_ptr<Row>&) -> mathgl::Vector4<float>;
  using Vector4DoubleAccessor = auto(const std::shared_ptr<Row>&) -> mathgl::Vector4<double>;

  /// \brief Maps table data using accessor function, returning a new array containing the mapped data.
  /// \param table Table to extract the data from.
  /// \param getValueFromRow Function that does the mapping on per-row basis.
  /// \return Resulting array data.
  static auto mapFloatColumn(const std::shared_ptr<arrow::Table>& table, std::function<FloatAccessor> getValueFromRow)
      -> std::shared_ptr<arrow::Array>;

  /// \brief Maps table data using accessor function, returning a new array containing the mapped data.
  /// \param table Table to extract the data from.
  /// \param getValueFromRow Function that does the mapping on per-row basis.
  /// \return Resulting array data.
  static auto mapVector3FloatColumn(const std::shared_ptr<arrow::Table>& table,
                                    std::function<Vector3FloatAccessor> getValueFromRow)
      -> std::shared_ptr<arrow::Array>;

  /// \brief Maps table data using accessor function, returning a new array containing the mapped data.
  /// \param table Table to extract the data from.
  /// \param getValueFromRow Function that does the mapping on per-row basis.
  /// \return Resulting array data.
  static auto mapVector3DoubleColumn(const std::shared_ptr<arrow::Table>& table,
                                     std::function<Vector3DoubleAccessor> getValueFromRow)
      -> std::shared_ptr<arrow::Array>;

  /// \brief Maps table data using accessor function, returning a new array containing the mapped data.
  /// \param table Table to extract the data from.
  /// \param getValueFromRow Function that does the mapping on per-row basis.
  /// \return Resulting array data.
  static auto mapVector4FloatColumn(const std::shared_ptr<arrow::Table>& table,
                                    std::function<Vector4FloatAccessor> getValueFromRow)
      -> std::shared_ptr<arrow::Array>;

  /// \brief Maps table data using accessor function, returning a new array containing the mapped data.
  /// \param table Table to extract the data from.
  /// \param getValueFromRow Function that does the mapping on per-row basis.
  /// \return Resulting array data.
  static auto mapVector4DoubleColumn(const std::shared_ptr<arrow::Table>& table,
                                     std::function<Vector4DoubleAccessor> getValueFromRow)
      -> std::shared_ptr<arrow::Array>;
};

}  // namespace deckgl

#endif  // DECKGL_CORE_ARROW_ARROW_TYPES_H
