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

TEST_F(JSONLoaderTest, ArrowTable) {
  auto input = std::shared_ptr<arrow::io::BufferReader>(new arrow::io::BufferReader(ndjsonDataSimple));

  std::shared_ptr<arrow::Table> table;
  ASSERT_NO_THROW({ table = jsonLoader->loadTable(input); });

  EXPECT_EQ(table->num_rows(), 1);
  EXPECT_EQ(table->num_columns(), 3);

  auto descriptions = std::static_pointer_cast<arrow::StringArray>(table->GetColumnByName("description")->chunk(0));
  EXPECT_EQ(descriptions->GetString(0), "Test");
}

}  // namespace
