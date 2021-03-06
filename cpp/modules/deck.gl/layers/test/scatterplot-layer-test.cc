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

#include "deck.gl/layers.h"

using namespace deckgl;

namespace {

/// \brief ScatterplotLayer test suite.
class ScatterplotLayerTest : public ::testing::Test {
 protected:
  ScatterplotLayerTest() { this->propData = this->_createPropDataTable(); }

  std::shared_ptr<arrow::Table> propData;

 private:
  auto _createPropDataTable() -> std::shared_ptr<arrow::Table> {
    arrow::MemoryPool* pool = arrow::default_memory_pool();

    // Positions
    arrow::FixedSizeListBuilder listBuilder{pool, std::make_shared<arrow::FloatBuilder>(pool), 3};
    arrow::FloatBuilder& valueBuilder = *(static_cast<arrow::FloatBuilder*>(listBuilder.value_builder()));

    EXPECT_TRUE(listBuilder.Append().ok());
    std::vector<float> position{1.2, 2.3, -3.4};
    EXPECT_TRUE(valueBuilder.AppendValues(position.data(), position.size()).ok());

    std::shared_ptr<arrow::Array> positions;
    EXPECT_TRUE(listBuilder.Finish(&positions).ok());

    std::vector<std::shared_ptr<arrow::Field>> fields = {
        arrow::field("position", arrow::fixed_size_list(arrow::float32(), 3))};

    auto schema = std::make_shared<arrow::Schema>(fields);
    return arrow::Table::Make(schema, {positions});
  }
};

TEST_F(ScatterplotLayerTest, Props) {
  auto layerProps1 = std::make_unique<ScatterplotLayer::Props>();
  auto layerProps2 = std::make_unique<ScatterplotLayer::Props>();

  EXPECT_TRUE(layerProps1->equals(layerProps2.get()));
  layerProps2->opacity = 0.5;
  EXPECT_FALSE(layerProps1->equals(layerProps2.get()));
}

TEST_F(ScatterplotLayerTest, GetPositionData) {
  auto layerProps = std::make_shared<ScatterplotLayer::Props>();
  layerProps->data = propData;

  auto layer = std::make_shared<ScatterplotLayer>(layerProps);
  auto positionData = layer->getPositionData(layerProps->data);

  EXPECT_EQ(positionData->length(), 1);
  EXPECT_EQ(positionData->type_id(), arrow::Type::FIXED_SIZE_LIST);

  auto listArray = std::static_pointer_cast<arrow::FixedSizeListArray>(positionData);
  auto values = std::static_pointer_cast<arrow::FloatArray>(listArray->values());
  EXPECT_EQ(values->length(), 3);
  EXPECT_EQ(values->type_id(), arrow::Type::FLOAT);

  // Buffer at index 0 contains null bitmap, the actual data is in the buffer at index 1 for majority of the data types
  auto listPointer = values->data()->GetValues<float>(1);
  EXPECT_FLOAT_EQ(*listPointer, 1.2);
  EXPECT_FLOAT_EQ(*(listPointer + 1), 2.3);
  EXPECT_FLOAT_EQ(*(listPointer + 2), -3.4);
}

TEST_F(ScatterplotLayerTest, GetRadiusData) {
  auto layerProps = std::make_shared<ScatterplotLayer::Props>();
  layerProps->data = propData;

  auto layer = std::make_shared<ScatterplotLayer>(layerProps);
  auto radiusData = layer->getRadiusData(layerProps->data);

  auto values = std::static_pointer_cast<arrow::FloatArray>(radiusData);
  EXPECT_EQ(values->length(), 1);
  EXPECT_EQ(values->type_id(), arrow::Type::FLOAT);

  EXPECT_FLOAT_EQ(values->Value(0), 1.0);
}

TEST_F(ScatterplotLayerTest, GetFillColorData) {
  auto layerProps = std::make_shared<ScatterplotLayer::Props>();
  layerProps->data = propData;

  auto layer = std::make_shared<ScatterplotLayer>(layerProps);
  auto colorData = layer->getFillColorData(layerProps->data);

  EXPECT_EQ(colorData->length(), 1);
  EXPECT_EQ(colorData->type_id(), arrow::Type::FIXED_SIZE_LIST);

  auto listArray = std::static_pointer_cast<arrow::FixedSizeListArray>(colorData);
  auto values = std::static_pointer_cast<arrow::FloatArray>(listArray->values());
  EXPECT_EQ(values->length(), 4);
  EXPECT_EQ(values->type_id(), arrow::Type::FLOAT);

  // Buffer at index 0 contains null bitmap, the actual data is in the buffer at index 1 for majority of the data types
  auto listPointer = values->data()->GetValues<float>(1);
  EXPECT_FLOAT_EQ(*listPointer, 0.0);
  EXPECT_FLOAT_EQ(*(listPointer + 1), 0.0);
  EXPECT_FLOAT_EQ(*(listPointer + 2), 0.0);
  EXPECT_FLOAT_EQ(*(listPointer + 3), 255.0);
}

TEST_F(ScatterplotLayerTest, GetLineColorData) {
  auto layerProps = std::make_shared<ScatterplotLayer::Props>();
  layerProps->data = propData;

  auto layer = std::make_shared<ScatterplotLayer>(layerProps);
  auto colorData = layer->getLineColorData(layerProps->data);

  EXPECT_EQ(colorData->length(), 1);
  EXPECT_EQ(colorData->type_id(), arrow::Type::FIXED_SIZE_LIST);

  auto listArray = std::static_pointer_cast<arrow::FixedSizeListArray>(colorData);
  auto values = std::static_pointer_cast<arrow::FloatArray>(listArray->values());
  EXPECT_EQ(values->length(), 4);
  EXPECT_EQ(values->type_id(), arrow::Type::FLOAT);

  // Buffer at index 0 contains null bitmap, the actual data is in the buffer at index 1 for majority of the data types
  auto listPointer = values->data()->GetValues<float>(1);
  EXPECT_FLOAT_EQ(*listPointer, 0.0);
  EXPECT_FLOAT_EQ(*(listPointer + 1), 0.0);
  EXPECT_FLOAT_EQ(*(listPointer + 2), 0.0);
  EXPECT_FLOAT_EQ(*(listPointer + 3), 255.0);
}

TEST_F(ScatterplotLayerTest, GetLineWidthData) {
  auto layerProps = std::make_shared<ScatterplotLayer::Props>();
  layerProps->data = propData;

  auto layer = std::make_shared<ScatterplotLayer>(layerProps);
  auto widthData = layer->getLineWidthData(layerProps->data);

  auto values = std::static_pointer_cast<arrow::FloatArray>(widthData);
  EXPECT_EQ(values->length(), 1);
  EXPECT_EQ(values->type_id(), arrow::Type::FLOAT);

  EXPECT_FLOAT_EQ(values->Value(0), 1.0);
}

}  // namespace
