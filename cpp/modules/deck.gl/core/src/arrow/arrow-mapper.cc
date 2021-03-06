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

#include "./arrow-mapper.h"  // NOLINT(build/include)

#include <arrow/builder.h>

using namespace deckgl;

auto ArrowMapper::mapBoolColumn(const std::shared_ptr<arrow::Table>& table, std::function<BoolAccessor> getValueFromRow)
    -> std::shared_ptr<arrow::Array> {
  arrow::MemoryPool* pool = arrow::default_memory_pool();
  arrow::BooleanBuilder builder{pool};

  for (auto i = 0; i < table->num_rows(); ++i) {
    // TODO(ilija@unfolded.ai): Move row object creation out of the loop
    auto row = Row{table, i};
    auto value = getValueFromRow(row);

    if (!builder.Append(value).ok()) {
      throw std::runtime_error("Unable to append vector data");
    }
  }

  std::shared_ptr<arrow::Array> resultArray;
  if (!builder.Finish(&resultArray).ok()) {
    throw std::runtime_error("Unable to append vector data");
  }

  return resultArray;
}

auto ArrowMapper::mapFloatColumn(const std::shared_ptr<arrow::Table>& table,
                                 std::function<FloatAccessor> getValueFromRow) -> std::shared_ptr<arrow::Array> {
  arrow::MemoryPool* pool = arrow::default_memory_pool();
  arrow::FloatBuilder builder{pool};

  for (auto i = 0; i < table->num_rows(); ++i) {
    // TODO(ilija@unfolded.ai): Move row object creation out of the loop
    auto row = Row{table, i};
    auto value = getValueFromRow(row);

    if (!builder.Append(value).ok()) {
      throw std::runtime_error("Unable to append vector data");
    }
  }

  std::shared_ptr<arrow::Array> resultArray;
  if (!builder.Finish(&resultArray).ok()) {
    throw std::runtime_error("Unable to append vector data");
  }

  return resultArray;
}

auto ArrowMapper::mapVector2FloatColumn(const std::shared_ptr<arrow::Table>& table,
                                        std::function<Vector2FloatAccessor> getValueFromRow)
    -> std::shared_ptr<arrow::Array> {
  arrow::MemoryPool* pool = arrow::default_memory_pool();
  arrow::FixedSizeListBuilder listBuilder{pool, std::make_shared<arrow::FloatBuilder>(pool), 2};
  arrow::FloatBuilder& valueBuilder = *(static_cast<arrow::FloatBuilder*>(listBuilder.value_builder()));

  for (auto i = 0; i < table->num_rows(); ++i) {
    // TODO(ilija@unfolded.ai): Move row object creation out of the loop
    auto row = Row{table, i};
    auto vector = getValueFromRow(row);

    if (!listBuilder.Append().ok()) {
      throw std::runtime_error("Unable to append vector data");
    }

    if (!valueBuilder.AppendValues(&vector.x, 2).ok()) {
      throw std::runtime_error("Unable to append vector data");
    }
  }

  std::shared_ptr<arrow::Array> resultArray;
  if (!listBuilder.Finish(&resultArray).ok()) {
    throw std::runtime_error("Unable to append vector data");
  }

  return resultArray;
}

auto ArrowMapper::mapVector3FloatColumn(const std::shared_ptr<arrow::Table>& table,
                                        std::function<Vector3FloatAccessor> getValueFromRow)
    -> std::shared_ptr<arrow::Array> {
  arrow::MemoryPool* pool = arrow::default_memory_pool();
  arrow::FixedSizeListBuilder listBuilder{pool, std::make_shared<arrow::FloatBuilder>(pool), 3};
  arrow::FloatBuilder& valueBuilder = *(static_cast<arrow::FloatBuilder*>(listBuilder.value_builder()));

  for (auto i = 0; i < table->num_rows(); ++i) {
    // TODO(ilija@unfolded.ai): Move row object creation out of the loop
    auto row = Row{table, i};
    auto vector = getValueFromRow(row);

    if (!listBuilder.Append().ok()) {
      throw std::runtime_error("Unable to append vector data");
    }

    if (!valueBuilder.AppendValues(&vector.x, 3).ok()) {
      throw std::runtime_error("Unable to append vector data");
    }
  }

  std::shared_ptr<arrow::Array> resultArray;
  if (!listBuilder.Finish(&resultArray).ok()) {
    throw std::runtime_error("Unable to append vector data");
  }

  return resultArray;
}

auto ArrowMapper::mapVector3DoubleColumn(const std::shared_ptr<arrow::Table>& table,
                                         std::function<Vector3DoubleAccessor> getValueFromRow)
    -> std::shared_ptr<arrow::Array> {
  arrow::MemoryPool* pool = arrow::default_memory_pool();
  arrow::FixedSizeListBuilder listBuilder{pool, std::make_shared<arrow::DoubleBuilder>(pool), 3};
  arrow::DoubleBuilder& valueBuilder = *(static_cast<arrow::DoubleBuilder*>(listBuilder.value_builder()));

  for (auto i = 0; i < table->num_rows(); ++i) {
    // TODO(ilija@unfolded.ai): Move row object creation out of the loop
    auto row = Row{table, i};
    auto vector = getValueFromRow(row);

    if (!listBuilder.Append().ok()) {
      throw std::runtime_error("Unable to append vector data");
    }

    if (!valueBuilder.AppendValues(&vector.x, 3).ok()) {
      throw std::runtime_error("Unable to append vector data");
    }
  }

  std::shared_ptr<arrow::Array> resultArray;
  if (!listBuilder.Finish(&resultArray).ok()) {
    throw std::runtime_error("Unable to append vector data");
  }

  return resultArray;
}

auto ArrowMapper::mapVector4FloatColumn(const std::shared_ptr<arrow::Table>& table,
                                        std::function<Vector4FloatAccessor> getValueFromRow)
    -> std::shared_ptr<arrow::Array> {
  arrow::MemoryPool* pool = arrow::default_memory_pool();
  arrow::FixedSizeListBuilder listBuilder{pool, std::make_shared<arrow::FloatBuilder>(pool), 4};
  arrow::FloatBuilder& valueBuilder = *(static_cast<arrow::FloatBuilder*>(listBuilder.value_builder()));

  for (auto i = 0; i < table->num_rows(); ++i) {
    // TODO(ilija@unfolded.ai): Move row object creation out of the loop
    auto row = Row{table, i};
    auto vector = getValueFromRow(row);

    if (!listBuilder.Append().ok()) {
      throw std::runtime_error("Unable to append vector data");
    }

    if (!valueBuilder.AppendValues(&vector.x, 4).ok()) {
      throw std::runtime_error("Unable to append vector data");
    }
  }

  std::shared_ptr<arrow::Array> resultArray;
  if (!listBuilder.Finish(&resultArray).ok()) {
    throw std::runtime_error("Unable to append vector data");
  }

  return resultArray;
}

auto ArrowMapper::mapVector4DoubleColumn(const std::shared_ptr<arrow::Table>& table,
                                         std::function<Vector4DoubleAccessor> getValueFromRow)
    -> std::shared_ptr<arrow::Array> {
  arrow::MemoryPool* pool = arrow::default_memory_pool();
  arrow::FixedSizeListBuilder listBuilder{pool, std::make_shared<arrow::DoubleBuilder>(pool), 3};
  arrow::DoubleBuilder& valueBuilder = *(static_cast<arrow::DoubleBuilder*>(listBuilder.value_builder()));

  for (auto i = 0; i < table->num_rows(); ++i) {
    // TODO(ilija@unfolded.ai): Move row object creation out of the loop
    auto row = Row{table, i};
    auto vector = getValueFromRow(row);

    if (!listBuilder.Append().ok()) {
      throw std::runtime_error("Unable to append vector data");
    }

    if (!valueBuilder.AppendValues(&vector.x, 4).ok()) {
      throw std::runtime_error("Unable to append vector data");
    }
  }

  std::shared_ptr<arrow::Array> resultArray;
  if (!listBuilder.Finish(&resultArray).ok()) {
    throw std::runtime_error("Unable to append vector data");
  }

  return resultArray;
}

auto ArrowMapper::mapListVector3FloatColumn(const std::shared_ptr<arrow::Table>& table,
                                            std::function<ListVector3FloatAccessor> getValueFromRow)
    -> std::shared_ptr<arrow::Array> {
  arrow::MemoryPool* pool = arrow::default_memory_pool();

  auto valueBuilder = std::make_shared<arrow::FloatBuilder>(pool);
  auto vectorBuilder = std::make_shared<arrow::FixedSizeListBuilder>(pool, valueBuilder, 3);
  auto listBuilder = std::make_shared<arrow::ListBuilder>(pool, vectorBuilder);

  for (auto i = 0; i < table->num_rows(); ++i) {
    // TODO(ilija@unfolded.ai): Move row object creation out of the loop
    auto row = Row{table, i};
    auto listData = getValueFromRow(row);

    if (!listBuilder->Append().ok()) {
      throw std::runtime_error("Unable to append vector data");
    }

    for (const auto& vectorData : listData) {
      if (!vectorBuilder->Append().ok()) {
        throw std::runtime_error("Unable to append vector data");
      }

      if (!valueBuilder->AppendValues(&vectorData.x, 3).ok()) {
        throw std::runtime_error("Unable to append vector data");
      }
    }
  }

  std::shared_ptr<arrow::Array> resultArray;
  if (!listBuilder->Finish(&resultArray).ok()) {
    throw std::runtime_error("Unable to append vector data");
  }

  return resultArray;
}
