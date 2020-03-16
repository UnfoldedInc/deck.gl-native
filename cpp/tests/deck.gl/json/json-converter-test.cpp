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
#include "deck.gl/json.h"
#include "deck.gl/layers.h"

using namespace deckgl;

namespace {

/**
 * The fixture for testing class Foo.
 */
class JSONConverterTest : public ::testing::Test {
 protected:
  // You can remove any or all of the following functions if their bodies would
  // be empty.
  std::unique_ptr<JSONConverter> jsonConverter;

  JSONConverterTest() {
    // You can do set-up work for each test here.
    jsonConverter = std::unique_ptr<JSONConverter>(new JSONConverter());

    jsonConverter->classes["Deck"] = [](const Json::Value &) -> std::shared_ptr<Component::Props> {
      return std::shared_ptr<Component::Props>{nullptr};
    };

    jsonConverter->classes["LineLayer"] = [](const Json::Value &) -> std::shared_ptr<Component::Props> {
      return std::shared_ptr<Component::Props>{new LineLayer::Props()};
    };
  }

  ~JSONConverterTest() override {
    // You can do clean-up work that doesn't throw exceptions here.
  }

  // If the constructor and destructor are not enough for setting up
  // and cleaning up each test, you can define the following methods:

  void SetUp() override {
    // Code here will be called immediately after the constructor (right
    // before each test).
  }

  void TearDown() override {
    // Code here will be called immediately after each test (right
    // before the destructor).
  }

  // Class members declared here can be used by all tests in the test suite
  // for Foo.
};

TEST_F(JSONConverterTest, JSONParseFail) { EXPECT_THROW(jsonConverter->parseJson(" ** BAD JSON"), std::runtime_error); }

TEST_F(JSONConverterTest, JSONParse) {
  EXPECT_NO_THROW({ Json::Value rootValue = jsonConverter->parseJson(jsonDataFull); });
}

TEST_F(JSONConverterTest, JSONConfig) {
  EXPECT_NO_THROW({
    Json::Value rootValue = jsonConverter->parseJson(jsonDataSimple);
    auto classConverter = jsonConverter->classes["LineLayer"];
    auto props = classConverter(rootValue);
  });
}

TEST_F(JSONConverterTest, JSONConverter) {
  Json::Value rootValue;
  EXPECT_NO_THROW({ rootValue = jsonConverter->parseJson(jsonDataSimple); });
  auto result = jsonConverter->convertJson(rootValue);

  std::cout << rootValue.get("mykey", "A Default Value if not exists").asString() << std::endl;
}

}  // namespace
