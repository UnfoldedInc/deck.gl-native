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

/// \brief Fixture for testing viewport-uniforms implementation.
class ViewportUniformsTest : public ::testing::Test {
 protected:
  ViewportUniformsTest() {}
};

TEST_F(ViewportUniformsTest, Simple) {
  // TODO(isaac@unfolded.ai): Add tests

  // TODO(isaac@unfolded.ai): Check for non-finite outputs
  // TODO(isaac@unfolded.ai): Check for coordinate origin (copy from JS)
}

// Ensures viewport uniform values are correct by comparing them to a known expected output.
// Input and output values taken from web-based deck.gl Flight Paths example.
TEST_F(ViewportUniformsTest, WebMercator) {
  WebMercatorViewport::Options opts;
  opts.width = 640;
  opts.height = 480;
  opts.longitude = 7.0;
  opts.latitude = 47.65;
  opts.zoom = 4.5;
  opts.pitch = 50.0;
  opts.bearing = 0.0;

  auto viewport = std::make_shared<WebMercatorViewport>(opts);
  auto uniforms = getUniformsFromViewport(viewport, 1, Matrix4<double>::MakeUnit(), COORDINATE_SYSTEM::DEFAULT,
                                          Vector3<double>{}, false);

  // clang-format off
  auto viewProjectionMatrix = Matrix4<double>{0.1060660171779, 0.0,              0.0,               -28.20884652413,
                                              0.0,             0.0909038955344,  0.1083350440839,   -30.29645116876,
                                              0.0,             0.03910417422475, -0.0328122981694,  -11.61662474340,
                                              0.0,             0.0361116813613,  -0.03030129851146, -10.53530150774};
  // clang-format on

  auto viewportSize = Vector2<float>{640.0, 480.0};

  EXPECT_EQ(uniforms.coordinateSystem, 1);
  EXPECT_EQ(uniforms.projectionMode, 1);
  EXPECT_FLOAT_EQ(uniforms.scale, 22.62741699796952f);
  //  EXPECT_EQ(uniforms.wrapLongitude, false);
  EXPECT_FLOAT_EQ(uniforms.antimeridian, -173.0f);
  EXPECT_EQ(uniforms.center, Vector4<float>{});
  EXPECT_EQ(uniforms.modelMatrix, Matrix4<float>::MakeUnit());
  EXPECT_EQ(uniforms.viewProjectionMatrix, Matrix4<float>{viewProjectionMatrix});
  EXPECT_EQ(uniforms.viewportSize, viewportSize);
  EXPECT_FLOAT_EQ(uniforms.devicePixelRatio, 1.0);
  EXPECT_FLOAT_EQ(uniforms.focalDistance, 1.0);
  EXPECT_EQ(uniforms.coordinateOrigin, Vector3<float>{});

  // Currently not calculated
  //  auto cameraPosition = Vector3<float>{265.9555555555556, 308.9046245972715, 20.45337649524195};
  //  EXPECT_EQ(uniforms.cameraPosition, cameraPosition);

  // Many small numbers that are hard to match consistently, should be tested as part of math.gl web-mercator-utils
  //  auto commonUnitsPerMeter = Vector3<float>{1.89e-05, 1.89e-05, 1.89e-05};
  //  auto commonUnitsPerWorldUnit = Vector3<float>{1.422222222222, 2.12548, 1.91e-05};
  //  auto commonUnitsPerWorldUnit2 = Vector3<float>{0.0, 0.0206, 3.70e-07};
  //
  //  EXPECT_EQ(uniforms.commonUnitsPerMeter, commonUnitsPerMeter);
  //  EXPECT_EQ(uniforms.commonUnitsPerWorldUnit, commonUnitsPerWorldUnit);
  //  EXPECT_EQ(uniforms.commonUnitsPerWorldUnit2, commonUnitsPerWorldUnit2);
}
