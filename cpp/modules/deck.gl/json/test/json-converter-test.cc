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

#include <gtest/gtest.h>

#include <iostream>
#include <stdexcept>
#include <string>

#include "./json-data.h"
#include "deck.gl/core.h"
#include "deck.gl/json.h"
#include "deck.gl/layers.h"

using namespace deckgl;

namespace {

/// \brief Fixture for testing class JSONConverter.
class JSONConverterTest : public ::testing::Test {
 protected:
  JSONConverterTest() {
    jsonConverter = std::make_unique<JSONConverter>();

    registerJSONConvertersForDeckCore(jsonConverter.get());
    registerJSONConvertersForDeckLayers(jsonConverter.get());
  }

  std::unique_ptr<JSONConverter> jsonConverter;
};

TEST_F(JSONConverterTest, JSONParseFail) { EXPECT_THROW(jsonConverter->parseJson(" ** BAD JSON"), std::runtime_error); }

TEST_F(JSONConverterTest, JSONParse) {
  EXPECT_NO_THROW({ Json::Value rootValue = jsonConverter->parseJson(jsonDataFull); });
}

TEST_F(JSONConverterTest, JSONConfig) {
  EXPECT_NO_THROW({
    Json::Value rootValue = jsonConverter->parseJson(jsonDataSimple);
    auto classConverter = jsonConverter->classes["LineLayer"];

    // TODO(ib@unfolded.ai): Bizarre test that parses a Deck as a LineLayer...
    auto lineLayerProps = classConverter(rootValue);
    EXPECT_TRUE(lineLayerProps);
    EXPECT_TRUE(std::dynamic_pointer_cast<LineLayer::Props>(lineLayerProps));
  });
}

TEST_F(JSONConverterTest, JSONConverterDeck) {
  Json::Value rootValue;
  rootValue = jsonConverter->parseJson(jsonDataSimple);
  auto jsonObject = jsonConverter->convertClass(rootValue, "Deck");

  EXPECT_TRUE(jsonObject);

  // Check that we get a deckProps object back
  auto deckProps = std::dynamic_pointer_cast<Deck::Props>(jsonObject);
  EXPECT_TRUE(deckProps);

  // Test deckProps.layers
  EXPECT_EQ(deckProps->layers.size(), size_t{2});

  auto layer0Props = deckProps->layers.front();
  EXPECT_TRUE(layer0Props);

  // Test deckProps->initialViewState
  EXPECT_NEAR(*deckProps->initialViewState->longitude, -122.45, 0.0001);
}

TEST_F(JSONConverterTest, JSONConverterDeckParsing) {
  auto result = jsonConverter->convertJson(jsonDataFull);
  EXPECT_TRUE(result);

  // Deck props
  auto deckProps = std::dynamic_pointer_cast<Deck::Props>(result);
  EXPECT_TRUE(deckProps);
  EXPECT_EQ(deckProps->id, "TestDeck");
}

TEST_F(JSONConverterTest, JSONConverterViewParsing) {
  auto result = jsonConverter->convertJson(jsonDataFull);
  EXPECT_TRUE(result);

  auto deckProps = std::dynamic_pointer_cast<Deck::Props>(result);
  EXPECT_TRUE(deckProps);

  auto views = deckProps->views;
  EXPECT_EQ(views.size(), size_t{2});

  auto view = views.front();
  EXPECT_EQ(view->id, "FirstView");
  EXPECT_EQ(view->x, 5);
  EXPECT_EQ(view->y, 10);
  EXPECT_EQ(view->width, 400);
  EXPECT_EQ(view->height, 300);
  EXPECT_DOUBLE_EQ(view->fovy, 70.0);
  EXPECT_DOUBLE_EQ(view->near, 1.0);
  EXPECT_DOUBLE_EQ(view->far, 500.0);
}

TEST_F(JSONConverterTest, JSONConverterViewStateParsing) {
  auto result = jsonConverter->convertJson(jsonDataFull);
  EXPECT_TRUE(result);

  auto deckProps = std::dynamic_pointer_cast<Deck::Props>(result);
  EXPECT_TRUE(deckProps);

  auto initialViewState = deckProps->initialViewState;
  EXPECT_DOUBLE_EQ(initialViewState->longitude.value(), -122.45);
  EXPECT_DOUBLE_EQ(initialViewState->latitude.value(), 37.8);
  EXPECT_DOUBLE_EQ(initialViewState->zoom.value(), 12.0);
  EXPECT_DOUBLE_EQ(initialViewState->bearing.value(), 20.0);
  EXPECT_DOUBLE_EQ(initialViewState->pitch.value(), 30.0);

  EXPECT_FALSE(deckProps->viewState);
}

TEST_F(JSONConverterTest, JSONConverterLayerParsing) {
  auto result = jsonConverter->convertJson(jsonDataFull);
  EXPECT_TRUE(result);

  auto deckProps = std::dynamic_pointer_cast<Deck::Props>(result);
  EXPECT_TRUE(deckProps);

  auto layer = std::dynamic_pointer_cast<Layer::Props>(deckProps->layers.front());
  EXPECT_TRUE(layer);

  EXPECT_EQ(layer->id, "LineLayer");
  EXPECT_EQ(layer->visible, false);
  EXPECT_FLOAT_EQ(layer->opacity, 0.5);
  mathgl::Vector3<double> coordinateOrigin{5.0, 3.0, 2.0};
  EXPECT_EQ(layer->coordinateOrigin, coordinateOrigin);
  EXPECT_EQ(layer->coordinateSystem, COORDINATE_SYSTEM::LNGLAT);
  EXPECT_EQ(layer->wrapLongitude, true);
  EXPECT_EQ(layer->positionFormat, "YZX");
  EXPECT_EQ(layer->colorFormat, "BGRA");
}

TEST_F(JSONConverterTest, JSONConverterLineLayerParsing) {
  auto result = jsonConverter->convertJson(jsonDataFull);
  EXPECT_TRUE(result);

  auto deckProps = std::dynamic_pointer_cast<Deck::Props>(result);
  EXPECT_TRUE(deckProps);

  auto lineLayer = std::dynamic_pointer_cast<LineLayer::Props>(deckProps->layers.front());
  EXPECT_TRUE(lineLayer);

  EXPECT_EQ(lineLayer->widthUnits, "meters");
  EXPECT_FLOAT_EQ(lineLayer->widthScale, 2.0);
  EXPECT_FLOAT_EQ(lineLayer->widthMinPixels, 1.0);
  EXPECT_FLOAT_EQ(lineLayer->widthMaxPixels, 100.0);
}

TEST_F(JSONConverterTest, JSONConverterScatterplotLayerParsing) {
  auto result = jsonConverter->convertJson(jsonDataFull);
  EXPECT_TRUE(result);

  auto deckProps = std::dynamic_pointer_cast<Deck::Props>(result);
  EXPECT_TRUE(deckProps);

  auto layerIterator = deckProps->layers.begin();
  std::advance(layerIterator, 1);
  auto scatterplotLayer = std::dynamic_pointer_cast<ScatterplotLayer::Props>(*layerIterator);
  EXPECT_TRUE(scatterplotLayer);

  EXPECT_EQ(scatterplotLayer->filled, false);
  EXPECT_EQ(scatterplotLayer->stroked, true);
  EXPECT_EQ(scatterplotLayer->lineWidthUnits, "pixels");
  EXPECT_FLOAT_EQ(scatterplotLayer->lineWidthScale, 2);
  EXPECT_FLOAT_EQ(scatterplotLayer->lineWidthMinPixels, 1);
  EXPECT_FLOAT_EQ(scatterplotLayer->lineWidthMaxPixels, 5.2);
  EXPECT_FLOAT_EQ(scatterplotLayer->radiusScale, 3);
  EXPECT_FLOAT_EQ(scatterplotLayer->radiusMinPixels, 1);
  EXPECT_FLOAT_EQ(scatterplotLayer->radiusMaxPixels, 10);
}

TEST_F(JSONConverterTest, JSONConverterSolidPolygonLayerParsing) {
  auto result = jsonConverter->convertJson(jsonDataFull);
  EXPECT_TRUE(result);

  auto deckProps = std::dynamic_pointer_cast<Deck::Props>(result);
  EXPECT_TRUE(deckProps);

  auto layerIterator = deckProps->layers.begin();
  std::advance(layerIterator, 2);
  auto solidPolygonLayer = std::dynamic_pointer_cast<SolidPolygonLayer::Props>(*layerIterator);
  EXPECT_TRUE(solidPolygonLayer);

  EXPECT_EQ(solidPolygonLayer->filled, false);
  EXPECT_FLOAT_EQ(solidPolygonLayer->elevationScale, 2);
}

}  // anonymous namespace
