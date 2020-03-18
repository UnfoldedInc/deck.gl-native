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

#include "math.gl/core.h"
#include "math.gl/web-mercator/web-mercator-utils.h"

using namespace mathgl;

namespace {
/**
 * The fixture for testing class JSONConverter.
 */
class WebMercatorUtilsTest : public ::testing::Test {
 protected:
  WebMercatorUtilsTest() {}
};

TEST_F(WebMercatorUtilsTest, lngLatToWorld) {
  auto input = Vector2<double>(-122, 38);
  auto output = lngLatToWorld(input);
  EXPECT_FLOAT_EQ(output.x, 82.4888888888889);
  EXPECT_FLOAT_EQ(output.y, 314.50692551385134);
}

// TODO: VIEWPORT_PROPS tests

TEST_F(WebMercatorUtilsTest, getMeterZoom) {
  const double TEST_LATITUDES[] = {0, 37.5, 75};

  for (auto latitude : TEST_LATITUDES) {
    auto zoom = getMeterZoom(latitude);
    auto scale = zoomToScale(zoom);

    auto distanceScales = getDistanceScales(Vector2<double>(latitude, 0));

    // zoom yields 1 pixel per meter
    EXPECT_FLOAT_EQ(distanceScales.unitsPerMeter.x * scale, 1);
    EXPECT_FLOAT_EQ(distanceScales.unitsPerMeter.y * scale, 1);
    EXPECT_FLOAT_EQ(distanceScales.unitsPerMeter.z * scale, 1);
  }
}

}  // namespace