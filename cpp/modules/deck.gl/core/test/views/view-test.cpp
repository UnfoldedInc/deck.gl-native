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

#include <memory>

#include "deck.gl/core.h"

#include "json-data.h"

using namespace mathgl;
using namespace deckgl;

class ViewTest : public ::testing::Test {
 protected:
  ViewTest() {
    jsonConverter = std::unique_ptr<JSONConverter>(new JSONConverter());

    registerJSONConvertersForDeckCore(jsonConverter.get());
  }

  std::unique_ptr<JSONConverter> jsonConverter;
};

TEST_F(ViewTest, JSONParse) {
  auto view1 = jsonConverter->convertJson(viewJsonData);
  auto view1copy = jsonConverter->convertJson(viewJsonData);

  // Tests for equality
  EXPECT_TRUE(view1->equals(view1));
  EXPECT_TRUE(view1->equals(view1copy));
  // pointers should not be the same even though the objects are equal
  EXPECT_FALSE(view1.get() == view1copy.get());
  EXPECT_FALSE(view1->equals(nullptr));
  EXPECT_EQ(view1->compare(view1), std::nullopt);
  EXPECT_EQ(view1->compare(view1copy), std::nullopt);
  EXPECT_NE(view1->compare(nullptr), std::nullopt);

  auto view2 = jsonConverter->convertJson(viewJsonDataWidth);

  EXPECT_FALSE(view1->equals(view2));
  EXPECT_NE(view1->compare(view2), std::nullopt);
}

TEST_F(ViewTest, JSONProps) {
  auto view = std::dynamic_pointer_cast<View>(jsonConverter->convertJson(viewJsonDataWidthAndHeight));

  EXPECT_EQ(view->x, 0);
  EXPECT_EQ(view->y, 0);
  EXPECT_EQ(view->width, 1000);
  EXPECT_EQ(view->height, 1000);
}
