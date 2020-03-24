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

using namespace mathgl;

/// \brief Utility class that provides a way to easily map Arrow tables.
class ArrowMapper {
 public:
  using FloatTransform = std::function<float(std::shared_ptr<Row>)>;
  /// \brief Maps table data using transform function, returning a new array containing the mapped data.
  /// \param table Table to extract the data from.
  /// \param transform Function that does the mapping on per-row basis.
  /// \return Resulting array data.
  static auto mapFloatColumn(const std::shared_ptr<arrow::Table>& table, FloatTransform transform)
      -> std::shared_ptr<arrow::Array>;

  using Vector3FloatTransform = std::function<Vector3<float>(std::shared_ptr<Row>)>;
  /// \brief Maps table data using transform function, returning a new array containing the mapped data.
  /// \param table Table to extract the data from.
  /// \param transform Function that does the mapping on per-row basis.
  /// \return Resulting array data.
  static auto mapVector3FloatColumn(const std::shared_ptr<arrow::Table>& table, Vector3FloatTransform transform)
      -> std::shared_ptr<arrow::Array>;

  using Vector3DoubleTransform = std::function<Vector3<double>(std::shared_ptr<Row>)>;
  /// \brief Maps table data using transform function, returning a new array containing the mapped data.
  /// \param table Table to extract the data from.
  /// \param transform Function that does the mapping on per-row basis.
  /// \return Resulting array data.
  static auto mapVector3DoubleColumn(const std::shared_ptr<arrow::Table>& table, Vector3DoubleTransform transform)
      -> std::shared_ptr<arrow::Array>;

  using Vector4FloatTransform = std::function<Vector4<float>(std::shared_ptr<Row>)>;
  /// \brief Maps table data using transform function, returning a new array containing the mapped data.
  /// \param table Table to extract the data from.
  /// \param transform Function that does the mapping on per-row basis.
  /// \return Resulting array data.
  static auto mapVector4FloatColumn(const std::shared_ptr<arrow::Table>& table, Vector4FloatTransform transform)
      -> std::shared_ptr<arrow::Array>;

  using Vector4DoubleTransform = std::function<Vector4<double>(std::shared_ptr<Row>)>;
  /// \brief Maps table data using transform function, returning a new array containing the mapped data.
  /// \param table Table to extract the data from.
  /// \param transform Function that does the mapping on per-row basis.
  /// \return Resulting array data.
  static auto mapVector4DoubleColumn(const std::shared_ptr<arrow::Table>& table, Vector4DoubleTransform transform)
      -> std::shared_ptr<arrow::Array>;
};

}  // namespace deckgl

#endif  // DECKGL_CORE_ARROW_ARROW_TYPES_H
