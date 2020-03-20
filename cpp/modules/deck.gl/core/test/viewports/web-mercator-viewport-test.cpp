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
auto const ALT_TOLERANCE = 1e-5;
auto const OFFSET_TOLERANCE = 1e-5;

const WebMercatorViewport TEST_VIEWPORTS[] = {WebMercatorViewport(800, 600, 38, -122, 11, 0, 0),
                                              WebMercatorViewport(800, 600, 23, 20, 15, 30, -85),
                                              WebMercatorViewport(800, 600, 65, 42, 16, 15, 30)};

TEST(WebMercatorViewport, Ctor) {
  WebMercatorViewport();
  // Construct with 0 width and height
  WebMercatorViewport(0, 0);
}

TEST(WebMercatorViewport, projectFlat) {
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

// TODO: project/unproject not implemented
// TEST(WebMercatorViewport, project3D) {
//   for (auto viewport : TEST_VIEWPORTS) {
//     const double TEST_OFFSETS[] = {0, 0.5, 1.0, 5.0};
//     for (auto offset : TEST_OFFSETS) {
//       auto lnglatIn3 = Vector3<double>(viewport.longitude + offset, viewport.latitude + offset, 0);
//       auto xyz3 = viewport.project(lnglatIn3);
//       auto lnglat3 = viewport.unproject(xyz3);

//       std::cout << lnglatIn3 << xyz3 << lnglat3 << std::endl;

//       EXPECT_NEAR(lnglatIn3.x, lnglat3.x, LNGLAT_TOLERANCE);
//       EXPECT_NEAR(lnglatIn3.y, lnglat3.y + 1, LNGLAT_TOLERANCE);
//       EXPECT_NEAR(lnglatIn3.z, lnglat3.z, ALT_TOLERANCE);
//     }
//   }
// }