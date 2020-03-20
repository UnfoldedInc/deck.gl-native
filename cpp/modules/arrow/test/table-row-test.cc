#include "../src/table-row.h"

#include <arrow/io/memory.h>
#include <arrow/table.h>
#include <gtest/gtest.h>

#include <memory>

#include "loaders.gl/json/src/json-loader.h"

namespace {

using namespace deckgl;
using namespace loadersgl;

/**
 * The fixture for testing class TableRow.
 */
class TableRowTest : public ::testing::Test {
 protected:
  TableRowTest() {
    // TODO: Ideally we have a common sample suite
    auto sampleData = R"(
    {"string":"sample1","int":1,"double":0.5}
    {"string":"sample2","int":2,"double":1.5}
    )";

    auto input = std::make_shared<arrow::io::BufferReader>(sampleData);
    table = this->jsonLoader->loadTable(input);
  }

  std::unique_ptr<JSONLoader> jsonLoader{std::make_unique<JSONLoader>()};
  std::shared_ptr<arrow::Table> table;
};

// Tests that TableRow retrieves data correctly.
TEST_F(TableRowTest, GetData) {
  auto row = std::make_unique<TableRow>(table, 0);

  EXPECT_EQ(row->getString("string"), "sample1");
  auto intValue = row->getNumber<arrow::Int64Type, int64_t>("int");
  EXPECT_EQ(intValue, 1);
  auto floatValue = row->getNumber<arrow::DoubleType, double>("double");
  EXPECT_FLOAT_EQ(floatValue, 0.5);

  row = std::make_unique<TableRow>(table, 1);
  EXPECT_EQ(row->getString("string"), "sample2");
  intValue = row->getNumber<arrow::Int64Type, int64_t>("int");
  EXPECT_EQ(intValue, 2);
  floatValue = row->getNumber<arrow::DoubleType, double>("double");
  EXPECT_FLOAT_EQ(floatValue, 1.5);

  // TODO(ilija): Can't get this one to compile for some reason...
  //  EXPECT_THROW({ row->getNumber<arrow::FloatType, float>("@@type"); }, std::logic_error);

  EXPECT_THROW({ std::make_unique<TableRow>(table, 2); }, std::range_error);
}

}  // namespace
