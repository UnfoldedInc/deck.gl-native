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

#include <algorithm>
#include <tuple>

#include "./sample-viewports.h"
#include "math.gl/core.h"
#include "math.gl/web-mercator.h"

using namespace mathgl;
using namespace std;

namespace {

auto const DISTANCE_TOLERANCE = 0.0005;
auto const DISTANCE_TOLERANCE_PIXELS = 2.0;
auto const DISTANCE_SCALE_TEST_ZOOM = 12.0;

auto getDiff(Vector3<double> value, Vector3<double> baseValue, double scale)
    -> std::tuple<Vector3<double>, Vector3<double>> {
  auto errorPixels = Vector3<double>(abs((value.x - baseValue.x) * scale), abs((value.y - baseValue.y) * scale),
                                     abs((value.z - baseValue.z) * scale));
  auto error = Vector3<double>(abs(value.x - baseValue.x) / min(abs(value.x), abs(baseValue.x)),
                               abs(value.y - baseValue.y) / min(abs(value.y), abs(baseValue.y)),
                               abs(value.z - baseValue.z) / min(abs(value.z), abs(baseValue.z)));
  return make_tuple(error, errorPixels);
}

TEST(WebMercatorUtils, lngLatToWorld) {
  auto input = Vector2<double>(-122, 38);
  auto output = lngLatToWorld(input);
  EXPECT_DOUBLE_EQ(output.x, 82.4888888888889);
  EXPECT_DOUBLE_EQ(output.y, 314.50692551385134);
}

TEST(WebMercatorUtils, getDistanceScales) {
  for (auto vc : SAMPLE_VIEWPORTS) {
    auto distanceScales = getDistanceScales(Vector2<double>(vc.longitude, vc.latitude));

    EXPECT_DOUBLE_EQ(distanceScales.metersPerUnit.x * distanceScales.unitsPerMeter.x, 1);
    EXPECT_DOUBLE_EQ(distanceScales.metersPerUnit.y * distanceScales.unitsPerMeter.y, 1);
    EXPECT_DOUBLE_EQ(distanceScales.metersPerUnit.z * distanceScales.unitsPerMeter.z, 1);

    EXPECT_DOUBLE_EQ(distanceScales.degreesPerUnit.x * distanceScales.unitsPerDegree.x, 1);
    EXPECT_DOUBLE_EQ(distanceScales.degreesPerUnit.y * distanceScales.unitsPerDegree.y, 1);
    EXPECT_DOUBLE_EQ(distanceScales.degreesPerUnit.z * distanceScales.unitsPerDegree.z, 1);
  }
}

TEST(WebMercatorUtils, getDistanceScales_unitsPerDegree) {
  auto scale = pow(2, DISTANCE_SCALE_TEST_ZOOM);
  auto z = 1000.0;

  for (auto vc : SAMPLE_VIEWPORTS) {
    auto distanceScales = getDistanceScales(Vector2<double>(vc.longitude, vc.latitude), true);

    // Test degree offsets
    const double TEST_DELTAS[] = {0.001, 0.01, 0.05, 0.1, 0.3};
    for (auto delta : TEST_DELTAS) {
      // To pixels
      auto coordsAdjusted =
          Vector3<double>(delta * (distanceScales.unitsPerDegree.x + distanceScales.unitsPerDegree2.x * delta),
                          delta * (distanceScales.unitsPerDegree.y + distanceScales.unitsPerDegree2.y * delta),
                          z * (distanceScales.unitsPerDegree.z + distanceScales.unitsPerDegree2.z * delta));
      auto pt = Vector2<double>(vc.longitude + delta, vc.latitude + delta);
      auto ptDistanceScales = getDistanceScales(pt);

      auto lngLatInWorld = lngLatToWorld(Vector2<double>(vc.longitude, vc.latitude));
      auto ptInWorld = lngLatToWorld(pt);
      auto realCoords = Vector3<double>(ptInWorld.x - lngLatInWorld.x, ptInWorld.y - lngLatInWorld.y,
                                        z * ptDistanceScales.unitsPerMeter.z);

      auto diffAdjusted = getDiff(coordsAdjusted, realCoords, scale);

      EXPECT_LT(get<0>(diffAdjusted).x, DISTANCE_TOLERANCE);
      EXPECT_LT(get<0>(diffAdjusted).y, DISTANCE_TOLERANCE);
      EXPECT_LT(get<0>(diffAdjusted).z, DISTANCE_TOLERANCE);

      EXPECT_LT(get<1>(diffAdjusted).x, DISTANCE_TOLERANCE_PIXELS);
      EXPECT_LT(get<1>(diffAdjusted).y, DISTANCE_TOLERANCE_PIXELS);
      EXPECT_LT(get<1>(diffAdjusted).z, DISTANCE_TOLERANCE_PIXELS);
    }
  }
}

TEST(WebMercatorUtils, getDistanceScales_unitsPerMeter) {
  auto scale = pow(2, DISTANCE_SCALE_TEST_ZOOM);
  auto z = 1000.0;

  for (auto vc : SAMPLE_VIEWPORTS) {
    auto distanceScales = getDistanceScales(Vector2<double>(vc.longitude, vc.latitude), true);

    // Test degree offsets
    const double TEST_DELTAS[] = {10, 100, 1000, 5000, 10000, 30000};
    for (auto i = 0; i < 6; i++) {
      auto delta = TEST_DELTAS[i];

      // To pixels
      auto coordsAdjusted =
          Vector3<double>(delta * (distanceScales.unitsPerMeter.x + distanceScales.unitsPerMeter2.x * delta),
                          delta * (distanceScales.unitsPerMeter.y + distanceScales.unitsPerMeter2.y * delta),
                          z * (distanceScales.unitsPerMeter.z + distanceScales.unitsPerMeter2.z * delta));
      auto realCoords = vc.unitsPerMeterResults[i];

      auto diffAdjusted = getDiff(coordsAdjusted, realCoords, scale);

      EXPECT_LT(get<0>(diffAdjusted).x, DISTANCE_TOLERANCE);
      EXPECT_LT(get<0>(diffAdjusted).y, DISTANCE_TOLERANCE);
      EXPECT_LT(get<0>(diffAdjusted).z, DISTANCE_TOLERANCE);

      EXPECT_LT(get<1>(diffAdjusted).x, DISTANCE_TOLERANCE_PIXELS);
      EXPECT_LT(get<1>(diffAdjusted).y, DISTANCE_TOLERANCE_PIXELS);
      EXPECT_LT(get<1>(diffAdjusted).z, DISTANCE_TOLERANCE_PIXELS);
    }
  }
}

TEST(WebMercatorUtils, addMetersToLngLat) {
  for (auto vc : SAMPLE_VIEWPORTS) {
    // Test degree offsets
    const double TEST_DELTAS[] = {10, 100, 1000, 5000};
    for (auto i = 0; i < 4; i++) {
      auto delta = TEST_DELTAS[i];
      auto origin = Vector2<double>(vc.longitude, vc.latitude);

      auto result = addMetersToLngLat(origin, Vector2<double>(delta, delta));

      // Extracted expected values from JS and test against those
      EXPECT_NEAR(result.x, vc.addMetersToLngLatResults[i].x, DISTANCE_TOLERANCE);
      EXPECT_NEAR(result.y, vc.addMetersToLngLatResults[i].y, DISTANCE_TOLERANCE);
    }
  }
}

TEST(WebMercatorUtils, getMeterZoom) {
  const double TEST_LATITUDES[] = {0, 37.5, 75};

  for (auto latitude : TEST_LATITUDES) {
    auto zoom = getMeterZoom(latitude);
    auto scale = zoomToScale(zoom);

    auto distanceScales = getDistanceScales(Vector2<double>(0, latitude));

    // zoom yields 1 pixel per meter
    EXPECT_NEAR(distanceScales.unitsPerMeter.x * scale, 1, DISTANCE_TOLERANCE);
    EXPECT_NEAR(distanceScales.unitsPerMeter.y * scale, 1, DISTANCE_TOLERANCE);
    EXPECT_NEAR(distanceScales.unitsPerMeter.z * scale, 1, DISTANCE_TOLERANCE);
  }
}

}  // namespace