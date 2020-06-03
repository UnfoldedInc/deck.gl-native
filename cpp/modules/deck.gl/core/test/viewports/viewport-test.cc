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

using namespace mathgl;
using namespace deckgl;

/// \brief Fixture for testing Viewport implementation.
class ViewportTest : public ::testing::Test {
 protected:
  ViewportTest() {}
};

TEST_F(ViewportTest, Simple) {
  ViewMatrixOptions viewMatrixOptions;
  ProjectionMatrixOptions projectionMatrixOptions;
  Viewport viewport{"my-viewport-id", viewMatrixOptions, projectionMatrixOptions, 0, 0, 0, 0};
  EXPECT_FALSE(viewport.containsPixel(0, 0));
  viewport.width = 10;
  viewport.height = 10;
  EXPECT_TRUE(viewport.containsPixel(2, 1, 5, 5));
}
