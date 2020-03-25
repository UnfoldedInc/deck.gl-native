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

#include "../../src/arrow/row.h"

#include <arrow/builder.h>
#include <arrow/table.h>
#include <gtest/gtest.h>

#include <memory>

namespace {

using namespace deckgl;

/// \brief The fixture for testing class Row.
class RowTest : public ::testing::Test {
 protected:
  RowTest() {
    arrow::MemoryPool* pool = arrow::default_memory_pool();

    // int column
    arrow::Int64Builder intBuilder{pool};
    EXPECT_TRUE(intBuilder.Append(42).ok());
    EXPECT_TRUE(intBuilder.Append(-212).ok());

    std::shared_ptr<arrow::Array> intArray;
    EXPECT_TRUE(intBuilder.Finish(&intArray).ok());

    // double column
    arrow::DoubleBuilder doubleBuilder{pool};
    EXPECT_TRUE(doubleBuilder.Append(-256.5).ok());
    EXPECT_TRUE(doubleBuilder.Append(991.1).ok());

    std::shared_ptr<arrow::Array> doubleArray;
    EXPECT_TRUE(doubleBuilder.Finish(&doubleArray).ok());

    // bool column
    arrow::BooleanBuilder boolBuilder{pool};
    EXPECT_TRUE(boolBuilder.Append(false).ok());

    std::shared_ptr<arrow::Array> boolArray;
    EXPECT_TRUE(boolBuilder.Finish(&boolArray).ok());

    // string column
    arrow::StringBuilder stringBuilder{pool};
    EXPECT_TRUE(stringBuilder.Append("first").ok());

    std::shared_ptr<arrow::Array> stringArray;
    EXPECT_TRUE(stringBuilder.Finish(&stringArray).ok());

    // float list column
    arrow::ListBuilder listBuilder{pool, std::make_shared<arrow::FloatBuilder>(pool)};
    arrow::FloatBuilder& listValueBuilder = *(static_cast<arrow::FloatBuilder*>(listBuilder.value_builder()));

    EXPECT_TRUE(listBuilder.Append().ok());
    std::vector<float> listValues{355.1, -26.1};
    EXPECT_TRUE(listValueBuilder.AppendValues(listValues.data(), listValues.size()).ok());

    std::shared_ptr<arrow::Array> listArray;
    EXPECT_TRUE(listBuilder.Finish(&listArray).ok());

    // double fixed list column
    arrow::FixedSizeListBuilder fixedListBuilder{pool, std::make_shared<arrow::DoubleBuilder>(pool), 3};
    arrow::DoubleBuilder& fixedListValueBuilder =
        *(static_cast<arrow::DoubleBuilder*>(fixedListBuilder.value_builder()));

    EXPECT_TRUE(fixedListBuilder.Append().ok());
    std::vector<double> fixedListValues{-256.2, 0.0, 1.23};
    EXPECT_TRUE(fixedListValueBuilder.AppendValues(fixedListValues.data(), fixedListValues.size()).ok());

    std::shared_ptr<arrow::Array> fixedListArray;
    EXPECT_TRUE(fixedListBuilder.Finish(&fixedListArray).ok());

    std::vector<std::shared_ptr<arrow::Field>> schemaVector = {
        arrow::field("int", arrow::int64()),
        arrow::field("double", arrow::float64()),
        arrow::field("bool", arrow::boolean()),
        arrow::field("string", arrow::utf8()),
        arrow::field("list", arrow::list(arrow::float32())),
        arrow::field("fixed_list", arrow::fixed_size_list(arrow::float64(), 3))};

    auto schema = std::make_shared<arrow::Schema>(schemaVector);
    table = arrow::Table::Make(schema, {intArray, doubleArray, boolArray, stringArray, listArray, fixedListArray});
  }

  std::shared_ptr<arrow::Table> table;
};

TEST_F(RowTest, ValidateInput) {
  ASSERT_EQ(table->num_columns(), 6);
  ASSERT_EQ(table->num_rows(), 2);
}

TEST_F(RowTest, Constructor) {
  EXPECT_THROW({ std::make_unique<Row>(table, 2); }, std::range_error);
}

TEST_F(RowTest, GetInt) {
  auto row = std::make_unique<Row>(table, 0);
  EXPECT_EQ(row->getInt("int"), 42);

  row = std::make_unique<Row>(table, 1);
  EXPECT_EQ(row->getInt("int"), -212);

  EXPECT_EQ(row->getInt("double"), 991);
}

TEST_F(RowTest, getDouble) {
  auto row = std::make_unique<Row>(table, 0);
  EXPECT_DOUBLE_EQ(row->getDouble("double"), -256.5);

  row = std::make_unique<Row>(table, 1);
  EXPECT_DOUBLE_EQ(row->getDouble("double"), 991.1);

  EXPECT_DOUBLE_EQ(row->getDouble("int"), -212.0);
}

TEST_F(RowTest, GetBool) {
  auto row = std::make_unique<Row>(table, 0);
  EXPECT_FALSE(row->getBool("bool"));

  row = std::make_unique<Row>(table, 1);
  EXPECT_FALSE(row->getBool("bool"));
}

TEST_F(RowTest, GetString) {
  auto row = std::make_unique<Row>(table, 0);
  EXPECT_EQ(row->getString("string", "default"), "first");

  row = std::make_unique<Row>(table, 1);
  EXPECT_EQ(row->getString("string"), "");
}

TEST_F(RowTest, getFloatVector2) {
  auto row = std::make_unique<Row>(table, 0);
  auto expectedVector = mathgl::Vector2<float>{355.1, -26.1};
  EXPECT_EQ(row->getFloatVector2("list"), expectedVector);

  row = std::make_unique<Row>(table, 1);
  expectedVector = mathgl::Vector2<float>{};
  EXPECT_EQ(row->getFloatVector2("list"), expectedVector);
}

TEST_F(RowTest, getDoubleVector3) {
  auto row = std::make_unique<Row>(table, 0);
  auto expectedVector = mathgl::Vector3<double>{-256.2, 0.0, 1.23};
  EXPECT_EQ(row->getDoubleVector3("fixed_list"), expectedVector);

  row = std::make_unique<Row>(table, 1);
  expectedVector = mathgl::Vector3<double>{1.0, 2.0, 3.0};
  EXPECT_EQ(row->getDoubleVector3("fixed_list", expectedVector), expectedVector);
}

TEST_F(RowTest, isValid) {
  auto row = std::make_unique<Row>(table, 1);
  EXPECT_TRUE(row->isValid("int"));
  EXPECT_FALSE(row->isValid("string"));
  EXPECT_FALSE(row->isValid("vector"));
}

}  // namespace
