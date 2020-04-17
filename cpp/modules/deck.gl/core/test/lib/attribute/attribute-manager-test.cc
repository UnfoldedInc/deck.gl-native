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

#include <arrow/builder.h>
#include <arrow/table.h>
#include <gtest/gtest.h>

#include <memory>

#include "deck.gl/core.h"
#include "deck.gl/layers.h"
#include "luma.gl/garrow.h"

using namespace deckgl;

namespace {

/// \brief The fixture for testing class AttributeManager.
class AttributeManagerTest : public ::testing::Test {
 protected:
  AttributeManagerTest() {
    this->manager = std::make_shared<AttributeManager>(this->managerId, nullptr);

    std::vector<std::shared_ptr<arrow::Field>> fields{};
    auto schema = std::make_shared<arrow::Schema>(fields);
    std::vector<std::shared_ptr<arrow::Array>> arrays{};
    this->emptyTable = arrow::Table::Make(schema, arrays);
  }

  std::shared_ptr<AttributeManager> manager;
  std::string managerId{"test-id"};
  std::shared_ptr<arrow::Table> emptyTable;
};

/// Tests that the AttributeManager initializes properly.
TEST_F(AttributeManagerTest, Initialization) { EXPECT_EQ(manager->id, managerId); }

/// Tests that AttributeManager sets redraw flag properly.
TEST_F(AttributeManagerTest, Redraw) {
  EXPECT_FALSE(manager->getNeedsRedraw());
  manager->setNeedsRedraw();
  EXPECT_TRUE(manager->getNeedsRedraw());
}

/// Tests that the update is performed correctly.
TEST_F(AttributeManagerTest, Update) {
  std::function<auto(const std::shared_ptr<arrow::Table>&)->std::shared_ptr<arrow::Array>> attributeUpdater{
      [](const std::shared_ptr<arrow::Table>& table) {
        arrow::MemoryPool* pool = arrow::default_memory_pool();
        arrow::FloatBuilder builder{pool};

        EXPECT_TRUE(builder.Append(1.0).ok());
        EXPECT_TRUE(builder.Append(-2.0).ok());
        EXPECT_TRUE(builder.Append(3.0).ok());

        std::shared_ptr<arrow::Array> resultArray;
        EXPECT_TRUE(builder.Finish(&resultArray).ok());

        return resultArray;
      }};
  manager->add(lumagl::garrow::AttributeDescriptor{"attribute-one", arrow::float32(), attributeUpdater});
  manager->add(lumagl::garrow::AttributeDescriptor{"attribute-two", arrow::float32(), attributeUpdater});

  // TODO(ilija@unfolded.ai): In order for this to run, we need to pass a valid Device instance to AttributeManager
  // Currently the only place where we do that is in GLFWAnimationLoop, which opens up a window which we don't want
  //  auto resultTable = manager->update(emptyTable);
  //  EXPECT_EQ(resultTable->num_rows(), 3);
  //  EXPECT_EQ(resultTable->num_columns(), 2);
  //  EXPECT_EQ(resultTable->ColumnNames()[0], "attribute-one");
  //  EXPECT_EQ(resultTable->ColumnNames()[1], "attribute-two");
}

}  // namespace
