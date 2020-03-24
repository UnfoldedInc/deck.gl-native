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

#include "./csv-data.h"
#include "loaders.gl/csv.h"

using namespace loadersgl;

namespace {

/**
 * The fixture for testing class CSVConvertet.
 */
class CSVLoaderTest : public ::testing::Test {
 protected:
  CSVLoaderTest(): csvLoader{std::unique_ptr<CSVLoader>()} {}

  std::unique_ptr<CSVLoader> csvLoader;
};

TEST_F(CSVLoaderTest, TypeCheck) {
  auto input = std::make_shared<arrow::io::BufferReader>(csvDataTypes);

  std::shared_ptr<arrow::Table> table;
  ASSERT_NO_THROW({ table = csvLoader->loadTable(input); });

  EXPECT_EQ(table->num_rows(), 4);
  EXPECT_EQ(table->num_columns(), 3);

  // Simplify extraction by expecting out of a single chunk. If test set grows past one chunk, revisit
  ASSERT_EQ(table->column(0)->num_chunks(), 1);

  auto ints = std::static_pointer_cast<arrow::Int64Array>(table->GetColumnByName("int")->chunk(0));
  EXPECT_EQ(ints->Value(0), -1234);
  EXPECT_EQ(ints->Value(1), 42);
  EXPECT_EQ(ints->Value(2), 55);
  EXPECT_EQ(ints->Value(3), 1);

  auto doubles = std::static_pointer_cast<arrow::DoubleArray>(table->GetColumnByName("double")->chunk(0));
  EXPECT_DOUBLE_EQ(doubles->Value(0), 1234.56);
  EXPECT_DOUBLE_EQ(doubles->Value(1), 942.2);
  EXPECT_DOUBLE_EQ(doubles->Value(2), 91);
  EXPECT_DOUBLE_EQ(doubles->Value(3), 995.0);

  auto strings = std::static_pointer_cast<arrow::StringArray>(table->GetColumnByName("string")->chunk(0));
  EXPECT_EQ(strings->GetString(0), "ValuE");
  EXPECT_EQ(strings->GetString(1), "Quoted, string");
  EXPECT_EQ(strings->GetString(2), "Another \"quoted\" string");
  EXPECT_EQ(strings->GetString(3), "String with\nline break");
}

TEST_F(CSVLoaderTest, LoadStates) {
  auto input = std::make_shared<arrow::io::BufferReader>(csvDataStates);

  std::shared_ptr<arrow::Table> table;
  ASSERT_NO_THROW({ table = csvLoader->loadTable(input); });

  EXPECT_EQ(table->num_rows(), 110);
  EXPECT_EQ(table->num_columns(), 3);

  auto aliases = std::static_pointer_cast<arrow::StringArray>(table->GetColumnByName("alias")->chunk(0));
  EXPECT_EQ(aliases->GetString(0), "AK");
}

}  // namespace
