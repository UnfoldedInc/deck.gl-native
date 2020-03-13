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

#include "deck.gl/core/viewports/viewport.h"

using namespace std;
using namespace mathgl;

ViewMatrixOptions::ViewMatrixOptions() : viewMatrix{Matrix4d() /*identity*/}, focalDistance{1} {}

ProjectionMatrixOptions::ProjectionMatrixOptions()
    : orthographic{false}, fovy{75}, near{0.1}, far{1000}, focalDistance{1} {}

Viewport::Viewport(const string& id, const ViewMatrixOptions& viewMatrixOptions,
                   const ProjectionMatrixOptions& projectionMatrixOptions, double x, double y, double width,
                   double height)
    : id{id}, x{x}, y{y}, width{width}, height{height} {}

auto Viewport::metersPerPixel() -> double { return this->distanceScaleMetersPerUnit.z / this->scale; }

auto Viewport::projectionMode() -> PROJECTION_MODE {
  if (this->isGeospatial) {
    return this->zoom < 12 ? PROJECTION_MODE::WEB_MERCATOR : PROJECTION_MODE::WEB_MERCATOR_AUTO_OFFSET;
  }
  return PROJECTION_MODE::IDENTITY;
}

auto Viewport::containsPixel(double x, double y, double width, double height) -> bool {
  return (x < this->x + this->width) && (this->x < x + width) && (y < this->y + this->height) && (this->y < y + height);
}

auto Viewport::getCameraPosition() -> mathgl::Vector3d { return this->cameraPosition; }

auto Viewport::getCameraDirection() -> mathgl::Vector3d { return this->cameraDirection; }

auto Viewport::getCameraUp() -> mathgl::Vector3d { return this->cameraUp; }

auto operator==(const Viewport& v1, const Viewport& v2) -> bool {
  return v1.width == v2.width && v1.height == v2.height && v1.scale == v2.scale &&
         v1.projectionMatrix == v2.projectionMatrix && v1.viewMatrix == v2.viewMatrix;
}

auto operator!=(const Viewport& v1, const Viewport& v2) -> bool { return !(v1 == v2); }
