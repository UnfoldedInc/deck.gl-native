// Copyright (c) 2020, Unfolded Inc
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

#include <arrow/array.h>
#include <arrow/io/memory.h>
#include <gtest/gtest.h>

#include <iostream>
#include <string>

#include "./json-loader-data.h"
#include "deck.gl/layers.h"
#include "loaders.gl/json.h"

using namespace loadersgl;

namespace {

/**
 * The fixture for testing class JSONLoader.
 */
class JSONLoaderTest : public ::testing::Test {
 protected:
  JSONLoaderTest() { jsonLoader = std::unique_ptr<JSONLoader>(new JSONLoader()); }

  std::unique_ptr<JSONLoader> jsonLoader;
};

TEST_F(JSONLoaderTest, TypeCheck) {
  auto input = std::make_shared<arrow::io::BufferReader>(jsonDataTypes);

  std::shared_ptr<arrow::Table> table;
  ASSERT_NO_THROW({ table = jsonLoader->loadTable(input); });

  // Simplify extraction by expecting out of a single chunk. If test set grows past one chunk, revisit
  ASSERT_EQ(table->column(0)->num_chunks(), 1);

  EXPECT_EQ(table->num_rows(), 2);
  EXPECT_EQ(table->num_columns(), 4);

  // string
  auto strings = std::static_pointer_cast<arrow::StringArray>(table->GetColumnByName("string")->chunk(0));
  EXPECT_EQ(strings->GetString(0), "sample1");
  EXPECT_EQ(strings->GetString(1), "sample2");

  // int
  auto ints = std::static_pointer_cast<arrow::Int64Array>(table->GetColumnByName("int")->chunk(0));
  EXPECT_EQ(ints->Value(0), -3351);
  EXPECT_EQ(ints->Value(1), 21);

  // double
  auto doubles = std::static_pointer_cast<arrow::DoubleArray>(table->GetColumnByName("double")->chunk(0));
  EXPECT_DOUBLE_EQ(doubles->Value(0), 65630.5);
  EXPECT_DOUBLE_EQ(doubles->Value(1), 5.0);

  // [double]
  auto vector2s = std::static_pointer_cast<arrow::ListArray>(table->GetColumnByName("double_array")->chunk(0));
  auto thisOffset = vector2s->value_offset(0);
  auto nextOffset = vector2s->value_offset(1);

  auto length = nextOffset - thisOffset;
  EXPECT_EQ(length, 2);

  auto values = std::static_pointer_cast<arrow::DoubleArray>(vector2s->values());
  EXPECT_EQ(values->type_id(), arrow::Type::DOUBLE);

  // Needed for some reason as seen in https://arrow.apache.org/docs/cpp/examples/row_columnar_conversion.html
  auto listPointer = values->data()->GetValues<double>(1);
  EXPECT_DOUBLE_EQ(*listPointer, 0.2);
  EXPECT_DOUBLE_EQ(*(listPointer + 1), 0.8);

  EXPECT_FALSE(vector2s->IsValid(1));
}

TEST_F(JSONLoaderTest, SimpleData) {
  auto input = std::make_shared<arrow::io::BufferReader>(ndjsonDataSimple);

  std::shared_ptr<arrow::Table> table;
  ASSERT_NO_THROW({ table = jsonLoader->loadTable(input); });

  EXPECT_EQ(table->num_rows(), 1);
  EXPECT_EQ(table->num_columns(), 3);

  auto descriptions = std::static_pointer_cast<arrow::StringArray>(table->GetColumnByName("description")->chunk(0));
  EXPECT_EQ(descriptions->GetString(0), "Test");
}

}  // namespace
