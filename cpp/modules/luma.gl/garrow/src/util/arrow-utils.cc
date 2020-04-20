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

#include "./arrow-utils.h"  // NOLINT(build/include)

#include "../table.h"

namespace lumagl {
namespace garrow {

auto vertexFormatFromArrowListType(const std::shared_ptr<arrow::FixedSizeListType>& type)
    -> std::optional<wgpu::VertexFormat>;

// TODO(ilija@unfolded.ai): Revisit and use maps instead

auto arrowTypeFromVertexFormat(wgpu::VertexFormat format) -> std::shared_ptr<arrow::DataType> {
  // Based on https://gpuweb.github.io/gpuweb/#vertex-formats
  // uchar = unsigned 8-bit value
  // char = signed 8-bit value
  // ushort = unsigned 16-bit value
  // short = signed 16-bit value
  // half = half-precision 16-bit floating point value
  // float = 32-bit floating point value
  // uint = unsigned 32-bit integer value
  // int = signed 32-bit integer value

  using Format = wgpu::VertexFormat;
  switch (format) {
    case Format::Char2:
    case Format::Char2Norm:
      return arrow::fixed_size_list(arrow::int8(), 2);
    case Format::UChar2:
    case Format::UChar2Norm:
      return arrow::fixed_size_list(arrow::uint8(), 2);
    case Format::Char4Norm:
    case Format::Char4:
      return arrow::fixed_size_list(arrow::int8(), 4);
    case Format::UChar4:
    case Format::UChar4Norm:
      return arrow::fixed_size_list(arrow::uint8(), 4);
    case Format::Short2:
    case Format::Short2Norm:
      return arrow::fixed_size_list(arrow::int16(), 2);
    case Format::UShort2:
    case Format::UShort2Norm:
      return arrow::fixed_size_list(arrow::uint16(), 2);
    case Format::Short4:
    case Format::Short4Norm:
      return arrow::fixed_size_list(arrow::int16(), 4);
    case Format::UShort4:
    case Format::UShort4Norm:
      return arrow::fixed_size_list(arrow::uint16(), 4);
    case Format::Half2:
      return arrow::fixed_size_list(arrow::float16(), 2);
    case Format::Half4:
      return arrow::fixed_size_list(arrow::float16(), 4);
    case Format::Float:
      return arrow::float32();
    case Format::Float2:
      return arrow::fixed_size_list(arrow::float32(), 2);
    case Format::Float3:
      return arrow::fixed_size_list(arrow::float32(), 3);
    case Format::Float4:
      return arrow::fixed_size_list(arrow::float32(), 4);
    case Format::UInt:
      return arrow::uint32();
    case Format::UInt2:
      return arrow::fixed_size_list(arrow::uint32(), 2);
    case Format::UInt3:
      return arrow::fixed_size_list(arrow::uint32(), 3);
    case Format::UInt4:
      return arrow::fixed_size_list(arrow::uint32(), 4);
    case Format::Int:
      return arrow::int32();
    case Format::Int2:
      return arrow::fixed_size_list(arrow::int32(), 2);
    case Format::Int3:
      return arrow::fixed_size_list(arrow::int32(), 3);
    case Format::Int4:
      return arrow::fixed_size_list(arrow::int32(), 4);
  }
}

auto vertexFormatFromArrowType(const std::shared_ptr<arrow::DataType>& type) -> std::optional<wgpu::VertexFormat> {
  using ArrowType = arrow::Type::type;
  using VertexFormat = wgpu::VertexFormat;

  switch (type->id()) {
    case ArrowType::UINT32:
      return VertexFormat::UInt;
    case ArrowType::INT32:
      return VertexFormat::Int;
    case ArrowType::FLOAT:
      return VertexFormat::Float;
    case ArrowType::FIXED_SIZE_LIST:
      if (auto listType = std::dynamic_pointer_cast<arrow::FixedSizeListType>(type)) {
        return vertexFormatFromArrowListType(listType);
      }
      break;
    default:
      // Rest of the types can't be mapped reasonably
      break;
  }

  return std::nullopt;
}

auto transformTable(const std::shared_ptr<arrow::Table>& table, const std::vector<ColumnBuilder>& builders,
                    wgpu::Device device) -> std::shared_ptr<Table> {
  std::vector<std::shared_ptr<Field>> fields;
  std::vector<std::shared_ptr<Array>> arrays;

  for (auto const& builder : builders) {
    auto field = builder.field;
    auto wgpuType = vertexFormatFromArrowType(field->type());
    if (!wgpuType.has_value()) {
      throw std::runtime_error("Unsupported data type");
    }

    std::shared_ptr<KeyValueMetadata> metadata = nullptr;
    if (field->HasMetadata()) {
      std::unordered_map<std::string, std::string> metadataMap;
      field->metadata()->ToUnorderedMap(&metadataMap);
      metadata = std::make_shared<KeyValueMetadata>(metadataMap);
    }
    auto gpuField = std::make_shared<Field>(builder.field->name(), wgpuType.value(), false, metadata);
    fields.push_back(gpuField);

    auto mappedArray = builder.mapColumn(table);
    auto gpuArray = std::make_shared<Array>(device, mappedArray);
    arrays.push_back(gpuArray);
  }

  auto schema = std::make_shared<Schema>(fields);
  return std::make_shared<Table>(schema, arrays);
}

auto vertexFormatFromArrowListType(const std::shared_ptr<arrow::FixedSizeListType>& type)
    -> std::optional<wgpu::VertexFormat> {
  using ArrowType = arrow::Type::type;
  using VertexFormat = wgpu::VertexFormat;

  switch (type->value_type()->id()) {
    case ArrowType::BOOL:
      switch (type->list_size()) {
        case 2:
          return VertexFormat::Char2;
        case 4:
          return VertexFormat::Char4;
        default:
          break;
      }
    case ArrowType::UINT16:
      switch (type->list_size()) {
        case 2:
          return VertexFormat::UShort2;
        case 4:
          return VertexFormat::UShort4;
        default:
          break;
      }
    case ArrowType::INT16:
      switch (type->list_size()) {
        case 2:
          return VertexFormat::Short2;
        case 4:
          return VertexFormat::Short4;
        default:
          break;
      }
    case ArrowType::UINT32:
      switch (type->list_size()) {
        case 2:
          return VertexFormat::UInt2;
        case 3:
          return VertexFormat::UInt3;
        case 4:
          return VertexFormat::UInt4;
        default:
          break;
      }
    case ArrowType::INT32:
      switch (type->list_size()) {
        case 2:
          return VertexFormat::Int2;
        case 3:
          return VertexFormat::Int3;
        case 4:
          return VertexFormat::Int4;
        default:
          break;
      }
    case ArrowType::HALF_FLOAT:
      switch (type->list_size()) {
        case 2:
          return VertexFormat::Half2;
        case 4:
          return VertexFormat::Half4;
        default:
          break;
      }
    case ArrowType::FLOAT:
      switch (type->list_size()) {
        case 2:
          return VertexFormat::Float2;
        case 3:
          return VertexFormat::Float3;
        case 4:
          return VertexFormat::Float4;
        default:
          break;
      }
    default:
      // Rest of the types can't be mapped reasonably
      break;
  }

  return std::nullopt;
}

}  // namespace garrow
}  // namespace lumagl
