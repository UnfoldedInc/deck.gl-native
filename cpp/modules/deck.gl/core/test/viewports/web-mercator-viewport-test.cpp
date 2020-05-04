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

auto const LNGLAT_TOLERANCE = 1e-6;
// auto const ALT_TOLERANCE = 1e-5;
auto const OFFSET_TOLERANCE = 1e-5;
auto const ZOOM_TOLERANCE = 1e-6;

auto makeTestViewport(int width, int height, double longitude, double latitude, double zoom, double pitch,
                      double bearing) -> WebMercatorViewport {
  WebMercatorViewport::Options opts;
  opts.width = width;
  opts.height = height;
  opts.longitude = longitude;
  opts.latitude = latitude;
  opts.zoom = zoom;
  opts.pitch = pitch;
  opts.bearing = bearing;

  return WebMercatorViewport{opts};
}

const WebMercatorViewport TEST_VIEWPORTS[] = {makeTestViewport(800, 600, -122, 38, 11, 0, 0),
                                              makeTestViewport(800, 600, 20, 23, 15, 30, -85),
                                              makeTestViewport(800, 600, 42, 65, 16, 15, 30)};

/// \brief Fixture for testing WebMercatorViewport implementation.
class WebMercatorViewportTest : public ::testing::Test {
 protected:
  WebMercatorViewportTest() {}
};

TEST_F(WebMercatorViewportTest, Simple) {
  ViewMatrixOptions viewMatrixOptions;
  ProjectionMatrixOptions projectionMatrixOptions;
  Viewport viewport{"my-viewport-id", viewMatrixOptions, projectionMatrixOptions, 0, 0, 0, 0};
  EXPECT_FALSE(viewport.containsPixel(0, 0));
  viewport.width = 10;
  viewport.height = 10;
  EXPECT_TRUE(viewport.containsPixel(2, 1, 5, 5));
}

TEST_F(WebMercatorViewportTest, Ctor) {
  // Construct with 0 width and height
  makeTestViewport(0, 0, 0, 0, 11, 0, 0);
}

TEST_F(WebMercatorViewportTest, projectFlat) {
  for (auto viewport : TEST_VIEWPORTS) {
    for (auto tc : TEST_VIEWPORTS) {
      auto lngLatIn = Vector2<double>(tc.longitude, tc.latitude);
      auto xy = viewport.projectFlat(lngLatIn);
      auto lngLat = viewport.unprojectFlat(xy);

      EXPECT_NEAR(lngLatIn.x, lngLat.x, LNGLAT_TOLERANCE);
      EXPECT_NEAR(lngLatIn.y, lngLat.y, LNGLAT_TOLERANCE);
    }
  }
}

// TODO(isaac@unfolded.ai): project/unproject not implemented
// TEST_F(WebMercatorViewportTest, project3D) {
//   for (auto viewport : TEST_VIEWPORTS) {
//     const double TEST_OFFSETS[] = {0, 0.5, 1.0, 5.0};
//     for (auto offset : TEST_OFFSETS) {
//       auto lnglatIn3 = Vector3<double>(viewport.longitude + offset, viewport.latitude + offset, 0);
//       auto xyz3 = viewport.project(lnglatIn3);
//       auto lnglat3 = viewport.unproject(xyz3);

//       std::cout << lnglatIn3 << xyz3 << lnglat3 << std::endl;

//       EXPECT_NEAR(lnglatIn3.x, lnglat3.x, LNGLAT_TOLERANCE);
//       EXPECT_NEAR(lnglatIn3.y, lnglat3.y, LNGLAT_TOLERANCE);
//       EXPECT_NEAR(lnglatIn3.z, lnglat3.z, ALT_TOLERANCE);
//     }
//   }
// }

// TODO(isaac@unfolded.ai): project/unproject not implemented
// TEST_F(WebMercatorViewportTest, project2D) {
//   for (auto viewport : TEST_VIEWPORTS) {
//     const double TEST_OFFSETS[] = {0, 0.5, 1.0, 5.0};
//     for (auto offset : TEST_OFFSETS) {
//       auto lnglatIn = Vector2<double>(viewport.longitude + offset, viewport.latitude + offset);
//       auto xy = viewport.project(lnglatIn);
//       auto lnglat = viewport.unproject(xy);

//       std::cout << lnglatIn3 << xyz3 << lnglat3 << std::endl;

//       EXPECT_NEAR(lnglatIn.x, lnglat.x, LNGLAT_TOLERANCE);
//       EXPECT_NEAR(lnglatIn.y, lnglat.y, LNGLAT_TOLERANCE);
//     }
//   }
// }

TEST_F(WebMercatorViewportTest, getScales) {
  for (auto viewport : TEST_VIEWPORTS) {
    auto distanceScales = viewport.getDistanceScales();

    EXPECT_NEAR(1, distanceScales.unitsPerMeter.x * distanceScales.metersPerUnit.x, LNGLAT_TOLERANCE);
    EXPECT_NEAR(1, distanceScales.unitsPerMeter.y * distanceScales.metersPerUnit.y, LNGLAT_TOLERANCE);
    EXPECT_NEAR(1, distanceScales.unitsPerMeter.z * distanceScales.metersPerUnit.z, LNGLAT_TOLERANCE);

    EXPECT_NEAR(1, distanceScales.unitsPerDegree.x * distanceScales.degreesPerUnit.x, LNGLAT_TOLERANCE);
    EXPECT_NEAR(1, distanceScales.unitsPerDegree.y * distanceScales.degreesPerUnit.y, LNGLAT_TOLERANCE);
    EXPECT_NEAR(1, distanceScales.unitsPerDegree.z * distanceScales.degreesPerUnit.z, LNGLAT_TOLERANCE);

    const double TEST_OFFSETS[] = {-0.01, 0.005, 0.01};
    for (auto offset : TEST_OFFSETS) {
      // In the test this doesn't actually define a third component - seems like it should be 2d
      auto xyz0 = Vector2<double>(viewport.center.x + distanceScales.unitsPerDegree.x * offset,
                                  viewport.center.y + distanceScales.unitsPerDegree.y * offset);
      auto xyz1 = viewport.projectFlat(Vector2<double>(viewport.longitude + offset, viewport.latitude + offset));

      EXPECT_NEAR(xyz0.x, xyz1.x, OFFSET_TOLERANCE);
      EXPECT_NEAR(xyz0.y, xyz1.y, OFFSET_TOLERANCE);
    }
  }
}

TEST_F(WebMercatorViewportTest, fitBounds) {
  struct fitBoundsArgs {
    Vector2<double> topLeft;
    Vector2<double> bottomRight;
    Vector2<int> offset;
    int padding;
    double minExtent;
    double maxZoom;
  };

  WebMercatorViewport TEST_VIEWPORT[] = {
      makeTestViewport(100, 100, -122.0, 37.7, 11.0, 0, 0), makeTestViewport(100, 100, -122.0, 37.7, 11.0, 0, 0),
      makeTestViewport(100, 100, -122.0, 37.7, 11.0, 0, 0), makeTestViewport(100, 100, -122.0, 37.7, 11.0, 0, 0),
      makeTestViewport(600, 400, -122.0, 37.7, 11.0, 0, 0) /*, makeTestViewport(600, 400, -122.0, 37.7, 11.0, 0, 0)*/};
  const WebMercatorViewport EXPECTED[] = {
      makeTestViewport(100, 100, -73.48759999999997, 41.26801443944763, 5.723804361273887, 0, 0),
      makeTestViewport(100, 100, -73.48759999999997, 41.26801443944763, 5.723804361273887, 0, 0),
      makeTestViewport(100, 100, -73.0, 10.0, 22.0, 0, 0), makeTestViewport(100, 100, -73.0, 10.0, 13.28771238, 0, 0),
      makeTestViewport(600, 400, -23.406499999999973, 64.86850056273362, 12.89199533073045, 0, 0)
      /*, makeTestViewport(600, 400, -23.406499999999973, 64.870857602, 12.476957831, 0, 0)*/};

  fitBoundsArgs TEST_ARGS[] = {
      {Vector2<double>(-73.9876, 40.7661), Vector2<double>(-72.9876, 41.7661), Vector2<int>(0, 0), 0, 0.0, 24.0},
      {Vector2<double>(-72.9876, 41.7661), Vector2<double>(-73.9876, 40.7661), Vector2<int>(0, 0), 0, 0.0, 24.0},
      {Vector2<double>(-73.0, 10.0), Vector2<double>(-73.0, 10.0), Vector2<int>(0, 0), 0, 0.0, 22.0},
      {Vector2<double>(-73.0, 10.0), Vector2<double>(-73.0, 10.0), Vector2<int>(0, 0), 0, 0.01, 24.0},
      {Vector2<double>(-23.407, 64.863), Vector2<double>(-23.406, 64.874), Vector2<int>(0, -40), 20, 0.0, 24.0}
      // , {Vector2<double>(-23.407, 64.863), Vector2<double>(-23.406, 64.874), Vector2<int>(0, -40), [100, 10, 30, 30],
      // 0.0, 24.0}
  };

  int i = 0;
  for (auto viewport : TEST_VIEWPORT) {
    fitBoundsArgs a = TEST_ARGS[i];
    WebMercatorViewport result =
        viewport.fitBounds(a.topLeft, a.bottomRight, a.minExtent, a.maxZoom, a.padding, a.offset);
    EXPECT_NEAR(result.longitude, EXPECTED[i].longitude, LNGLAT_TOLERANCE);
    EXPECT_NEAR(result.latitude, EXPECTED[i].latitude, LNGLAT_TOLERANCE);
    EXPECT_NEAR(result.zoom, EXPECTED[i].zoom, ZOOM_TOLERANCE);
    i++;
  }
}

// Ensures viewport matrix values are correct by comparing them to a known expected output.
// Input and output values taken from web-based deck.gl Flight Paths example.
TEST_F(WebMercatorViewportTest, FlightPathsMatrices) {
  auto viewport = makeTestViewport(640, 480, 7.0, 47.65, 4.5, 50.0, 0.0);

  auto projectionMatrix = Matrix4<double>{
      2.25, 0.0, 0.0, 0.0, 0.0, 3.0, 0.0, 0.0, 0.0, 0.0, -1.082867724532, -0.2082867724532, 0.0, 0.0, -1.0, 0.0};

  // clang-format off
  auto viewMatrix = Matrix4<double>{0.0471404520791, 0.0,              0.0,              -12.53726512183,
                                    0.0,             0.03030129851146, 0.0361116813613,  -10.09881705625,
                                    0.0,             -0.0361116813613, 0.03030129851146, 10.53530150774,
                                    0.0,             0.0,              0.0,              1.0};

  auto viewProjectionMatrix = Matrix4<double>{0.1060660171779, 0.0,              0.0,               -28.20884652413,
                                              0.0,             0.0909038955344,  0.1083350440839,   -30.29645116876,
                                              0.0,             0.03910417422475, -0.0328122981694,  -11.61662474340,
                                              0.0,             0.0361116813613,  -0.03030129851146, -10.53530150774};
  // clang-format on

  EXPECT_EQ(viewport.projectionMatrix, projectionMatrix);
  EXPECT_EQ(viewport.viewMatrix, viewMatrix);
  EXPECT_EQ(viewport.viewProjectionMatrix, viewProjectionMatrix);
}
