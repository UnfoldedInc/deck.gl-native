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

using deckgl::Deck;
using deckgl::JSONConverter;
using deckgl::LineLayer;
using deckgl::ScatterplotLayer;

namespace {

/**
 * The fixture for testing class JSONConverter.
 */
class JSONConverterTest : public ::testing::Test {
 protected:
  JSONConverterTest() {
    jsonConverter = std::unique_ptr<JSONConverter>(new JSONConverter());

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

    // TODO(ib): bizarre test that parses a Deck as a LineLayer...
    auto lineLayerProps = classConverter(rootValue);
    std::cout << lineLayerProps->getProperties()->className << std::endl;
    EXPECT_TRUE(lineLayerProps);
    EXPECT_TRUE(std::dynamic_pointer_cast<LineLayer::Props>(lineLayerProps));
  });
}

TEST_F(JSONConverterTest, JSONConverter) {
  Json::Value rootValue;
  rootValue = jsonConverter->parseJson(jsonDataSimple);
  auto result = jsonConverter->convertJson(rootValue);
  // EXPECT_TRUE(result);
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
  EXPECT_EQ(deckProps->layers.size(), 2);

  auto layer0Props = deckProps->layers.front();
  EXPECT_TRUE(layer0Props);
  std::cout << "Layer: " << layer0Props->getProperties()->className << std::endl;

  // Test deckProps->initialViewState
  EXPECT_NEAR(*deckProps->initialViewState->longitude, -122.45, 0.0001);
  std::cout << "Done" << std::endl;
}

}  // namespace
