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

#include "math.gl/core.h"

#include <gtest/gtest.h>

namespace {

/// \brief The fixture for testing core math classes.
class MathTest : public ::testing::Test {
 protected:
  MathTest() {}
};

TEST_F(MathTest, Vector3Sum) {
  mathgl::Vector3 vector1{0.1, 0.3, 0.5};
  mathgl::Vector3 vector2{0.6, 0.2, 0.1};

  mathgl::Vector3 vectorSum = vector1 + vector2;

  EXPECT_FLOAT_EQ(vectorSum.x, 0.7);
  EXPECT_FLOAT_EQ(vectorSum.y, 0.5);
  EXPECT_FLOAT_EQ(vectorSum.z, 0.6);
}

}  // namespace
