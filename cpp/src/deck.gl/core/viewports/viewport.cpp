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

Viewport::Viewport() : x(0), y(0), width(0), height(0) {}

double Viewport::metersPerPixel() { return this->distanceScaleMetersPerUnit.z / this->scale; }

PROJECTION_MODE Viewport::projectionMode() {
  if (this->isGeospatial) {
    return this->zoom < 12 ? PROJECTION_MODE::WEB_MERCATOR : PROJECTION_MODE::WEB_MERCATOR_AUTO_OFFSET;
  }
  return PROJECTION_MODE::IDENTITY;
}

bool Viewport::containsPixel(double x, double y, double width, double height) {
  return x < this->x + this->width && this->x < x + width && y < this->y + this->height && this->y < y + height;
}

mathgl::Vector3d Viewport::getCameraPosition() { return this->cameraPosition; }

mathgl::Vector3d Viewport::getCameraDirection() { return this->cameraDirection; }

mathgl::Vector3d Viewport::getCameraUp() { return this->cameraUp; }

bool operator==(const Viewport& v1, const Viewport& v2) {
  return v1.width == v2.width && v1.height == v2.height && v1.scale == v2.scale &&
         v1.porjectionMatrix == v2.porjectionMatrix && v1.viewMatrix == v2.viewMatrix;
}

bool operator!=(const Viewport& v1, const Viewport& v2) { return !(v1 == v2); }
