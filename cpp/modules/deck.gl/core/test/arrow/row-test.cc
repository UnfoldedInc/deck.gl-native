#include "../../src/arrow/row.h"

#include <arrow/io/memory.h>
#include <arrow/table.h>
#include <gtest/gtest.h>

#include <memory>

#include "loaders.gl/json/src/json-loader.h"

namespace {

using namespace deckgl;
using namespace loadersgl;

/**
 * The fixture for testing class Row.
 */
class RowTest : public ::testing::Test {
 protected:
  RowTest() {
    // TODO: Ideally we have a common sample suite
    auto sampleData = R"(
    {"string":"sample1","int":1,"double":0.5,"vector2":[0.2,0.8]}
    {"string":"sample2","int":2,"double":1.5,"vector3":[1.1, 2.2, 3.3]}
    )";

    auto input = std::make_shared<arrow::io::BufferReader>(sampleData);
    table = this->jsonLoader->loadTable(input);
  }

  std::unique_ptr<JSONLoader> jsonLoader{std::make_unique<JSONLoader>()};
  std::shared_ptr<arrow::Table> table;
};

// Tests that Row retrieves data correctly.
TEST_F(RowTest, Accessors) {
  auto row = std::make_unique<Row>(table, 0);
  EXPECT_EQ(row->getString("string"), "sample1");
  EXPECT_EQ(row->getInt("int"), 1);
  EXPECT_FLOAT_EQ(row->getDouble("double"), 0.5);

  auto expectedVector2 = mathgl::Vector2<double>{0.2, 0.8};
  EXPECT_EQ(row->getDoubleVector2("vector2"), expectedVector2);
  auto expectedVector3 = mathgl::Vector3<double>{};
  EXPECT_EQ(row->getDoubleVector3("vector3"), expectedVector3);

  row = std::make_unique<Row>(table, 1);
  EXPECT_EQ(row->getString("string"), "sample2");
  EXPECT_EQ(row->getInt("int"), 2);
  EXPECT_FLOAT_EQ(row->getDouble("double"), 1.5);

  expectedVector2 = mathgl::Vector2<double>{};
  EXPECT_EQ(row->getDoubleVector2("vector2"), expectedVector2);
  expectedVector3 = mathgl::Vector3<double>{1.1, 2.2, 3.3};
  EXPECT_EQ(row->getDoubleVector3("vector3"), expectedVector3);

  EXPECT_THROW({ row->getFloat("@@type"); }, std::logic_error);

  EXPECT_THROW({ std::make_unique<Row>(table, 2); }, std::range_error);
}

}  // namespace
