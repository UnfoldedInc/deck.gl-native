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

/// \brief SolidPolygonLayer test suite.
class SolidPolygonLayerTest : public ::testing::Test {
 protected:
  SolidPolygonLayerTest() { this->propData = this->_createPropDataTable(); }

  std::shared_ptr<arrow::Table> propData;

 private:
  auto _createPropDataTable() -> std::shared_ptr<arrow::Table> {
    arrow::MemoryPool* pool = arrow::default_memory_pool();

    // Polygons
    auto componentBuilder = std::make_shared<arrow::FloatBuilder>(pool);
    auto pointBuilder = std::make_shared<arrow::FixedSizeListBuilder>(pool, componentBuilder, 3);
    auto polygonBuilder = std::make_shared<arrow::ListBuilder>(pool, pointBuilder);

    EXPECT_TRUE(polygonBuilder->Append().ok());

    EXPECT_TRUE(pointBuilder->Append().ok());
    std::vector<float> point{1.2, 2.3, -3.4};
    EXPECT_TRUE(componentBuilder->AppendValues(point.data(), point.size()).ok());

    EXPECT_TRUE(pointBuilder->Append().ok());
    point = {2.1, 3.2, -4.3};
    EXPECT_TRUE(componentBuilder->AppendValues(point.data(), point.size()).ok());

    EXPECT_TRUE(pointBuilder->Append().ok());
    point = {-1.2, 2.3, 3.4};
    EXPECT_TRUE(componentBuilder->AppendValues(point.data(), point.size()).ok());

    std::shared_ptr<arrow::Array> polygons;
    EXPECT_TRUE(polygonBuilder->Finish(&polygons).ok());

    // Elevation
    arrow::FloatBuilder elevationBuilder{pool};
    EXPECT_TRUE(elevationBuilder.Append(2.5).ok());

    std::shared_ptr<arrow::Array> elevations;
    EXPECT_TRUE(elevationBuilder.Finish(&elevations).ok());

    // Fill Color
    arrow::FixedSizeListBuilder fillColorListBuilder{pool, std::make_shared<arrow::FloatBuilder>(pool), 4};
    arrow::FloatBuilder& fillColorBuilder = *(static_cast<arrow::FloatBuilder*>(fillColorListBuilder.value_builder()));

    EXPECT_TRUE(fillColorListBuilder.Append().ok());
    std::vector<float> color{-4.3, 3.2, -2.1, 0.1};
    EXPECT_TRUE(fillColorBuilder.AppendValues(color.data(), color.size()).ok());

    std::shared_ptr<arrow::Array> fillColors;
    EXPECT_TRUE(fillColorListBuilder.Finish(&fillColors).ok());

    // Line Color
    arrow::FixedSizeListBuilder lineColorListBuilder{pool, std::make_shared<arrow::FloatBuilder>(pool), 4};
    arrow::FloatBuilder& lineColorBuilder = *(static_cast<arrow::FloatBuilder*>(lineColorListBuilder.value_builder()));

    EXPECT_TRUE(lineColorListBuilder.Append().ok());
    color = {-4.3, 3.2, -2.1, 0.1};
    EXPECT_TRUE(lineColorBuilder.AppendValues(color.data(), color.size()).ok());

    std::shared_ptr<arrow::Array> lineColors;
    EXPECT_TRUE(lineColorListBuilder.Finish(&lineColors).ok());

    std::vector<std::shared_ptr<arrow::Field>> schemaVector = {
        arrow::field("polygon", arrow::list(arrow::fixed_size_list(arrow::float32(), 3))),
        arrow::field("elevation", arrow::float32()),
        arrow::field("fillColor", arrow::fixed_size_list(arrow::float32(), 4)),
        arrow::field("lineColor", arrow::fixed_size_list(arrow::float32(), 4))};

    auto schema = std::make_shared<arrow::Schema>(schemaVector);
    return arrow::Table::Make(schema, {polygons, elevations, fillColors, lineColors});
  }
};

TEST_F(SolidPolygonLayerTest, PropComparison) {
  auto layerProps1 = std::make_shared<SolidPolygonLayer::Props>();
  auto layerProps2 = std::make_shared<SolidPolygonLayer::Props>();

  EXPECT_TRUE(layerProps1->equals(layerProps2.get()));
  layerProps2->opacity = 0.5;
  layerProps2->elevationScale = 0.5;
  EXPECT_FALSE(layerProps1->equals(layerProps2.get()));
}

TEST_F(SolidPolygonLayerTest, PropAccess) {
  auto layerProps1 = std::make_shared<SolidPolygonLayer::Props>();

  auto propertyTypes = layerProps1->getProperties();
  EXPECT_TRUE(propertyTypes->hasProp("opacity"));
  EXPECT_TRUE(propertyTypes->hasProp("filled"));
  EXPECT_FALSE(propertyTypes->hasProp("radiusScale"));
}

TEST_F(SolidPolygonLayerTest, Create) {
  auto layerProps = std::make_shared<SolidPolygonLayer::Props>();
  auto polygonLayer = std::make_unique<SolidPolygonLayer>(layerProps);

  EXPECT_EQ(polygonLayer->props(), layerProps);
}

TEST_F(SolidPolygonLayerTest, GetPolygonData) {
  // TODO(ilija@unfolded.ai): Props should be mocked in order to completely decouple this test from props implementation
  auto layerProps = std::make_shared<SolidPolygonLayer::Props>();
  layerProps->data = propData;

  auto polygonLayer = std::make_shared<SolidPolygonLayer>(layerProps);
  auto processedData = polygonLayer->processData(propData);
  auto positionData = polygonLayer->getPositionData(processedData);

  EXPECT_EQ(positionData->length(), 3);
  EXPECT_EQ(positionData->type_id(), arrow::Type::FIXED_SIZE_LIST);

  auto listArray = std::static_pointer_cast<arrow::FixedSizeListArray>(positionData);
  auto values = std::static_pointer_cast<arrow::FloatArray>(listArray->values());
  EXPECT_EQ(values->length(), 9);
  EXPECT_EQ(values->type_id(), arrow::Type::FLOAT);

  // Buffer at index 0 contains null bitmap, the actual data is in the buffer at index 1 for majority of the data types
  auto dataPointer = values->data()->GetValues<float>(1);
  EXPECT_FLOAT_EQ(*dataPointer, 1.2);
  EXPECT_FLOAT_EQ(*(dataPointer + 1), 2.3);
  EXPECT_FLOAT_EQ(*(dataPointer + 2), -3.4);
  EXPECT_FLOAT_EQ(*(dataPointer + 3), 2.1);
}

TEST_F(SolidPolygonLayerTest, GetElevationData) {
  // TODO(ilija@unfolded.ai): Props should be mocked in order to completely decouple this test from props implementation
  auto layerProps = std::make_shared<SolidPolygonLayer::Props>();
  layerProps->data = propData;

  auto polygonLayer = std::make_shared<SolidPolygonLayer>(layerProps);
  auto processedData = polygonLayer->processData(propData);
  auto elevationData = polygonLayer->getElevationData(processedData);

  auto values = std::static_pointer_cast<arrow::FloatArray>(elevationData);
  EXPECT_EQ(values->length(), 3);
  EXPECT_EQ(values->type_id(), arrow::Type::FLOAT);

  EXPECT_FLOAT_EQ(values->Value(0), 1000.0);
}

TEST_F(SolidPolygonLayerTest, GetFillColorData) {
  // TODO(ilija@unfolded.ai): Props should be mocked in order to completely decouple this test from props implementation
  auto layerProps = std::make_shared<SolidPolygonLayer::Props>();
  layerProps->data = propData;

  auto polygonLayer = std::make_shared<SolidPolygonLayer>(layerProps);
  auto processedData = polygonLayer->processData(propData);
  auto colorData = polygonLayer->getFillColorData(processedData);

  EXPECT_EQ(colorData->length(), 3);
  EXPECT_EQ(colorData->type_id(), arrow::Type::FIXED_SIZE_LIST);

  auto listArray = std::static_pointer_cast<arrow::FixedSizeListArray>(colorData);
  auto values = std::static_pointer_cast<arrow::FloatArray>(listArray->values());
  EXPECT_EQ(values->length(), 12);
  EXPECT_EQ(values->type_id(), arrow::Type::FLOAT);

  // Buffer at index 0 contains null bitmap, the actual data is in the buffer at index 1 for majority of the data types
  auto dataPointer = values->data()->GetValues<float>(1);
  EXPECT_FLOAT_EQ(*dataPointer, 0.0);
  EXPECT_FLOAT_EQ(*(dataPointer + 1), 0.0);
  EXPECT_FLOAT_EQ(*(dataPointer + 2), 0.0);
  EXPECT_FLOAT_EQ(*(dataPointer + 3), 255.0);
}

TEST_F(SolidPolygonLayerTest, GetLineColorData) {
  // TODO(ilija@unfolded.ai): Props should be mocked in order to completely decouple this test from props implementation
  auto layerProps = std::make_shared<SolidPolygonLayer::Props>();
  layerProps->data = propData;

  auto polygonLayer = std::make_shared<SolidPolygonLayer>(layerProps);
  auto processedData = polygonLayer->processData(propData);
  auto colorData = polygonLayer->getLineColorData(processedData);

  EXPECT_EQ(colorData->length(), 3);
  EXPECT_EQ(colorData->type_id(), arrow::Type::FIXED_SIZE_LIST);

  auto listArray = std::static_pointer_cast<arrow::FixedSizeListArray>(colorData);
  auto values = std::static_pointer_cast<arrow::FloatArray>(listArray->values());
  EXPECT_EQ(values->length(), 12);
  EXPECT_EQ(values->type_id(), arrow::Type::FLOAT);

  // Buffer at index 0 contains null bitmap, the actual data is in the buffer at index 1 for majority of the data types
  auto dataPointer = values->data()->GetValues<float>(1);
  EXPECT_FLOAT_EQ(*dataPointer, 0.0);
  EXPECT_FLOAT_EQ(*(dataPointer + 1), 0.0);
  EXPECT_FLOAT_EQ(*(dataPointer + 2), 0.0);
  EXPECT_FLOAT_EQ(*(dataPointer + 3), 255.0);
}

}  // namespace
