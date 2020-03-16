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

#include "./web-mercator-utils.h"

namespace mathgl {

ViewMatrixOptions::ViewMatrixOptions() : viewMatrix{Matrix4d() /*identity*/}, focalDistance{1} {}

ProjectionMatrixOptions::ProjectionMatrixOptions()
    : orthographic{false}, fovy{75}, near{0.1}, far{1000}, focalDistance{1} {}

auto zoomToScale(double zoom) -> double { return pow(2, zoom); }

auto scaleToZoom(double scale) -> double { return log2(scale); }

auto lngLatToWorld(Vector2d lngLat) -> Vector2d {
  if (lngLat.y < -90 || lngLat.y > 90) {
    throw std::logic_error("invalid latitude");
  }

  auto lambda2 = lngLat.x * DEGREES_TO_RADIANS;
  auto phi2 = lngLat.y * DEGREES_TO_RADIANS;
  auto x = (TILE_SIZE * (lambda2 + PI)) / (2 * PI);
  auto y = (TILE_SIZE * (PI + log(tan(PI_4 + phi2 * 0.5)))) / (2 * PI);
  return {x, y};
}

auto worldToLngLat(Vector2d xy) -> Vector2d {
  auto lambda2 = (xy.x / TILE_SIZE) * (2 * PI) - PI;
  auto phi2 = 2 * (atan(exp((xy.y / TILE_SIZE) * (2 * PI) - PI)) - PI_4);
  return {lambda2 * RADIANS_TO_DEGREES, phi2 * RADIANS_TO_DEGREES};
}

auto getMeterZoom(double latitude) -> double {
  auto latCosine = cos(latitude * DEGREES_TO_RADIANS);
  return scaleToZoom(EARTH_CIRCUMFERENCE * latCosine) - 9;
}

}  // namespace mathgl
