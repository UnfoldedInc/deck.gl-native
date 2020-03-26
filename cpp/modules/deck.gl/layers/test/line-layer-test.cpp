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

/// \brief LineLayer test suite.
class LineLayerTest : public ::testing::Test {
 protected:
  LineLayerTest() { this->propData = this->_createPropDataTable(); }

  std::shared_ptr<arrow::Table> propData;

 private:
  auto _createPropDataTable() -> std::shared_ptr<arrow::Table> {
    arrow::MemoryPool* pool = arrow::default_memory_pool();

    // Positions
    arrow::FixedSizeListBuilder listBuilder{pool, std::make_shared<arrow::DoubleBuilder>(pool), 3};
    arrow::DoubleBuilder& valueBuilder = *(static_cast<arrow::DoubleBuilder*>(listBuilder.value_builder()));

    EXPECT_TRUE(listBuilder.Append().ok());
    std::vector<double> position{1.2, 2.3, -3.4};
    EXPECT_TRUE(valueBuilder.AppendValues(position.data(), position.size()).ok());

    std::shared_ptr<arrow::Array> positions;
    EXPECT_TRUE(listBuilder.Finish(&positions).ok());

    // Color
    arrow::FixedSizeListBuilder colorListBuilder{pool, std::make_shared<arrow::FloatBuilder>(pool), 4};
    arrow::FloatBuilder& colorValueBuilder = *(static_cast<arrow::FloatBuilder*>(colorListBuilder.value_builder()));

    EXPECT_TRUE(colorListBuilder.Append().ok());
    std::vector<float> color{-4.3, 3.2, -2.1, 0.1};
    EXPECT_TRUE(colorValueBuilder.AppendValues(color.data(), color.size()).ok());

    std::shared_ptr<arrow::Array> colors;
    EXPECT_TRUE(colorListBuilder.Finish(&colors).ok());

    // Width
    arrow::FloatBuilder widthBuilder{pool};
    EXPECT_TRUE(widthBuilder.Append(2.5).ok());

    std::shared_ptr<arrow::Array> widths;
    EXPECT_TRUE(widthBuilder.Finish(&widths).ok());

    std::vector<std::shared_ptr<arrow::Field>> schemaVector = {
        arrow::field("sourcePosition", arrow::fixed_size_list(arrow::float64(), 3)),
        arrow::field("targetPosition", arrow::fixed_size_list(arrow::float64(), 3)),
        arrow::field("color", arrow::fixed_size_list(arrow::float32(), 4)), arrow::field("width", arrow::float32())};

    auto schema = std::make_shared<arrow::Schema>(schemaVector);
    return arrow::Table::Make(schema, {positions, positions, colors, widths});
  }
};

TEST_F(LineLayerTest, PropComparison) {
  auto layerProps1 = std::make_shared<LineLayer::Props>();
  auto layerProps2 = std::make_shared<LineLayer::Props>();

  EXPECT_TRUE(layerProps1->equals(layerProps2.get()));
  layerProps2->opacity = 0.5;
  layerProps2->widthScale = 0.5;
  EXPECT_FALSE(layerProps1->equals(layerProps2.get()));
}

TEST_F(LineLayerTest, PropAccess) {
  auto layerProps1 = std::make_shared<LineLayer::Props>();

  auto propertyTypes = layerProps1->getProperties();
  EXPECT_TRUE(propertyTypes->hasProp("opacity"));
  EXPECT_TRUE(propertyTypes->hasProp("widthScale"));
  EXPECT_FALSE(propertyTypes->hasProp("radiusScale"));
}

TEST_F(LineLayerTest, Create) {
  auto layerProps = std::make_shared<LineLayer::Props>();

  auto lineLayer = new LineLayer(layerProps);

  EXPECT_EQ(lineLayer->props(), layerProps);
}

TEST_F(LineLayerTest, GetSourcePositionData) {
  // TODO(ilija@unfolded.ai): Props should be mocked in order for this test to be decoupled and make sense
  auto layerProps = std::make_shared<LineLayer::Props>();
  layerProps->data = propData;

  auto lineLayer = std::make_shared<LineLayer>(layerProps);
  auto positionData = lineLayer->getSourcePositionData(layerProps->data);

  EXPECT_EQ(positionData->length(), 1);
  EXPECT_EQ(positionData->type_id(), arrow::Type::FIXED_SIZE_LIST);

  auto listArray = std::static_pointer_cast<arrow::FixedSizeListArray>(positionData);
  auto values = std::static_pointer_cast<arrow::DoubleArray>(listArray->values());
  EXPECT_EQ(values->length(), 3);
  EXPECT_EQ(values->type_id(), arrow::Type::DOUBLE);

  // Needed for some reason as seen in https://arrow.apache.org/docs/cpp/examples/row_columnar_conversion.html
  auto listPointer = values->data()->GetValues<double>(1);
  EXPECT_DOUBLE_EQ(*listPointer, 1.2);
  EXPECT_DOUBLE_EQ(*(listPointer + 1), 2.3);
  EXPECT_DOUBLE_EQ(*(listPointer + 2), -3.4);
}

TEST_F(LineLayerTest, GetTargetPositionData) {
  // TODO(ilija@unfolded.ai): Props should be mocked in order for this test to be decoupled and make sense
  auto layerProps = std::make_shared<LineLayer::Props>();
  layerProps->data = propData;

  auto lineLayer = std::make_shared<LineLayer>(layerProps);
  auto positionData = lineLayer->getTargetPositionData(layerProps->data);

  EXPECT_EQ(positionData->length(), 1);
  EXPECT_EQ(positionData->type_id(), arrow::Type::FIXED_SIZE_LIST);

  auto listArray = std::static_pointer_cast<arrow::FixedSizeListArray>(positionData);
  auto values = std::static_pointer_cast<arrow::DoubleArray>(listArray->values());
  EXPECT_EQ(values->length(), 3);
  EXPECT_EQ(values->type_id(), arrow::Type::DOUBLE);

  // Needed for some reason as seen in https://arrow.apache.org/docs/cpp/examples/row_columnar_conversion.html
  auto listPointer = values->data()->GetValues<double>(1);
  EXPECT_DOUBLE_EQ(*listPointer, 1.2);
  EXPECT_DOUBLE_EQ(*(listPointer + 1), 2.3);
  EXPECT_DOUBLE_EQ(*(listPointer + 2), -3.4);
}

TEST_F(LineLayerTest, GetColorData) {
  // TODO(ilija@unfolded.ai): Props should be mocked in order for this test to be decoupled and make sense
  auto layerProps = std::make_shared<LineLayer::Props>();
  layerProps->data = propData;

  auto lineLayer = std::make_shared<LineLayer>(layerProps);
  auto colorData = lineLayer->getColorData(layerProps->data);

  EXPECT_EQ(colorData->length(), 1);
  EXPECT_EQ(colorData->type_id(), arrow::Type::FIXED_SIZE_LIST);

  auto listArray = std::static_pointer_cast<arrow::FixedSizeListArray>(colorData);
  auto values = std::static_pointer_cast<arrow::FloatArray>(listArray->values());
  EXPECT_EQ(values->length(), 4);
  EXPECT_EQ(values->type_id(), arrow::Type::FLOAT);

  // Needed for some reason as seen in https://arrow.apache.org/docs/cpp/examples/row_columnar_conversion.html
  auto listPointer = values->data()->GetValues<float>(1);
  EXPECT_FLOAT_EQ(*listPointer, 0.0);
  EXPECT_FLOAT_EQ(*(listPointer + 1), 0.0);
  EXPECT_FLOAT_EQ(*(listPointer + 2), 0.0);
  EXPECT_FLOAT_EQ(*(listPointer + 3), 255.0);
}

TEST_F(LineLayerTest, GetWidthData) {
  // TODO(ilija@unfolded.ai): Props should be mocked in order for this test to be decoupled and make sense
  auto layerProps = std::make_shared<LineLayer::Props>();
  layerProps->data = propData;

  auto lineLayer = std::make_shared<LineLayer>(layerProps);
  auto widthData = lineLayer->getWidthData(layerProps->data);

  auto values = std::static_pointer_cast<arrow::FloatArray>(widthData);
  EXPECT_EQ(values->length(), 1);
  EXPECT_EQ(values->type_id(), arrow::Type::FLOAT);

  EXPECT_FLOAT_EQ(values->Value(0), 1.0);
}

}  // namespace
