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

#include "deck.gl/layers.h"

using namespace deckgl;

namespace {

TEST(LineLayer, PropComparison) {
  auto layerProps1 = std::unique_ptr<LineLayer::Props>(new LineLayer::Props());
  auto layerProps2 = std::unique_ptr<LineLayer::Props>(new LineLayer::Props());

  EXPECT_TRUE(layerProps1->compare(layerProps2.get()));
  layerProps2->opacity = 0.5;
  layerProps2->widthScale = 0.5;
  EXPECT_FALSE(layerProps1->compare(layerProps2.get()));
}

TEST(LineLayer, PropAccess) {
  auto layerProps1 = std::unique_ptr<LineLayer::Props>(new LineLayer::Props());

  auto propTypes = layerProps1->getPropTypes();
  EXPECT_TRUE(propTypes->count("opacity") == 1);
  EXPECT_TRUE(propTypes->count("widthScale") == 1);
  EXPECT_TRUE(propTypes->count("radiusScale") == 0);
}

}  // namespace
