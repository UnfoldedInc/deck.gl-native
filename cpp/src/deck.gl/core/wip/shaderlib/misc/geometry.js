<<<<<<< HEAD:cpp/src/core/shaderlib/misc/geometry.js
// Copyright (c) 2015 - 2017 Uber Technologies, Inc.
=======
// Copyright (c) 2020, Unfolded Inc
>>>>>>> prop-types: can now iterate over and compare layer props:cpp/tests/core/lib/layer.spec.cpp
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

<<<<<<< HEAD:cpp/src/core/shaderlib/misc/geometry.js
const vs = `
struct VertexGeometry {
  vec4 position;
  vec3 worldPosition;
  vec3 worldPositionAlt;
  vec3 normal;
  vec2 uv;
  vec3 pickingColor;
} geometry;
`;

const fs = `
#define SMOOTH_EDGE_RADIUS 0.5

struct FragmentGeometry {
  vec2 uv;
} geometry;

float smoothedge(float edge, float x) {
  return smoothstep(edge - SMOOTH_EDGE_RADIUS, edge + SMOOTH_EDGE_RADIUS, x);
}
`;

export default {name: 'geometry', vs, fs};
=======
#include <gtest/gtest.h>

#include "../../../src/core/lib/layer.h"

TEST(Layer, Props) {
  auto layerProps1 = new LayerProps();
  auto layerProps2 = new LayerProps();

  EXPECT_TRUE(layerProps1->compare(layerProps2));
  layerProps2->opacity = 0.5;
  EXPECT_FALSE(layerProps1->compare(layerProps2));
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
>>>>>>> prop-types: can now iterate over and compare layer props:cpp/tests/core/lib/layer.spec.cpp
