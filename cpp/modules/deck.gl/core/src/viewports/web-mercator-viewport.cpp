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

#include "./web-mercator-viewport.h"  // NOLINT(build/include)

#include <algorithm>

#include "math.gl/web-mercator.h"

using namespace std;
using namespace mathgl;
using namespace deckgl;

ViewMatrixOptions calculateViewMatrixOptions(int height, double longitude, double latitude, double pitch,
                                             double bearing, double altitude, double zoom) {
  auto scale = pow(2, zoom);
  height = height == 0 ? 1 : height;

  // The uncentered matrix allows us two move the center addition to the
  // shader (cheap) which gives a coordinate system that has its center in
  // the layer's center position. This makes rotations and other modelMatrx
  // transforms much more useful.
  auto viewMatrixUncentered = getViewMatrix(height, pitch, bearing, altitude, scale);

  auto opts = ViewMatrixOptions();
  opts.viewMatrix = viewMatrixUncentered;
  opts.isGeospatial = true;  // TODO(isaac): ???
  opts.longitude = longitude;
  opts.latitude = latitude;
  opts.zoom = zoom;
  opts.position = Vector3<double>();
  // opts.modelMatrix // TODO(isaac):
  // opts.distanceScales // TODO(isaac):
  return opts;
}

ProjectionMatrixOptions calculateProjectionMatrixOptions(int width, int height, double pitch, double altitude,
                                                         double nearZMultiplier, double farZMultiplier) {
  width = width == 0 ? 1 : width;
  height = height == 0 ? 1 : height;

  // Altitude - prevent division by 0
  // TODO(isaac): - just throw an Error instead?
  altitude = max(0.75, altitude);

  return getProjectionParameters(width, height, altitude, pitch, nearZMultiplier, farZMultiplier);
}

WebMercatorViewport::WebMercatorViewport(int width, int height, double longitude, double latitude, double zoom,
                                         double pitch, double bearing, double altitude, double nearZMultiplier,
                                         double farZMultiplier, bool orthographic, bool repeat, double worldOffset
                                         // Silently allow apps to send in 0,0 to facilitate isomorphic render etc
                                         )
    : Viewport("web-mercator-viewport",
               calculateViewMatrixOptions(height, longitude, latitude, pitch, bearing, altitude, zoom),
               calculateProjectionMatrixOptions(width, height, pitch, altitude, nearZMultiplier, farZMultiplier), 0, 0,
               width == 0 ? 1 : width, height == 0 ? 1 : height) {
  // TODO(isaac): Need to cleanup the call to the superclass ctor. In JS this is done partway through this ctor,
  // but that can't be done in C++. Perhaps just call a non-virtual _init method instead?

  // TODO(isaac):
  // if (worldOffset) {
  //   const viewOffset = new Matrix4().translate([512 * worldOffset, 0, 0]);
  //   viewMatrixUncentered = viewOffset.multiplyLeft(viewMatrixUncentered);
  // }

  // this->_subViewports = repeat ? [] : null;
}

auto WebMercatorViewport::addMetersToLngLat(mathgl::Vector3<double> lngLatZ, mathgl::Vector3<double> xyz)
    -> mathgl::Vector3<double> {
  return mathgl::addMetersToLngLat(lngLatZ, xyz);
}

auto WebMercatorViewport::addMetersToLngLat(mathgl::Vector2<double> lngLat, mathgl::Vector2<double> xy)
    -> mathgl::Vector2<double> {
  return mathgl::addMetersToLngLat(lngLat, xy);
}

auto WebMercatorViewport::getMapCenterByLngLatPosition(mathgl::Vector2<double> lngLat, mathgl::Vector2<double> pos)
    -> mathgl::Vector2<double> {
  auto fromLocation = pixelsToWorld(pos, this->pixelUnprojectionMatrix);
  auto toLocation = this->projectFlat(lngLat);

  auto translate = toLocation - fromLocation;
  auto newCenter = this->center.toVector2() + translate;

  return this->unprojectFlat(newCenter);
}

auto fitBounds(mathgl::Vector2<double> topLeft, mathgl::Vector2<double> bottomRight, int padding = 0,
               mathgl::Vector2<int> offset = mathgl::Vector2<int>()) {
  throw new std::logic_error("fitBounds not supported");
}
