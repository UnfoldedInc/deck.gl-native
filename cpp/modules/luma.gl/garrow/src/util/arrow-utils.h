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

#ifndef LUMAGL_GARROW_UTIL_ARROW_UTILS_H
#define LUMAGL_GARROW_UTIL_ARROW_UTILS_H

#include <arrow/api.h>
#include <dawn/webgpu_cpp.h>

#include <memory>
#include <optional>
#include <vector>

namespace lumagl {
namespace garrow {

class Table;
struct AttributeDescriptor;

struct ColumnBuilder {
  using ColumnMapping = auto(const std::shared_ptr<arrow::Table>&) -> std::shared_ptr<arrow::Array>;

  ColumnBuilder(const std::shared_ptr<arrow::Field>& field, const std::function<ColumnMapping>& mapColumn)
      : field{field}, mapColumn{mapColumn} {}

  std::shared_ptr<arrow::Field> field;
  std::function<ColumnMapping> mapColumn;
};

auto arrowTypeFromVertexFormat(wgpu::VertexFormat format) -> std::shared_ptr<arrow::DataType>;
auto vertexFormatFromArrowType(const std::shared_ptr<arrow::DataType>& type) -> std::optional<wgpu::VertexFormat>;

auto transformTable(const std::shared_ptr<arrow::Table>& table, const std::vector<ColumnBuilder>& builders,
                    wgpu::Device device) -> std::shared_ptr<Table>;

}  // namespace garrow
}  // namespace lumagl

#endif  // LUMAGL_GARROW_UTIL_ARROW_UTILS_H
