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

#include "./viewport.h"
#include "math.gl/web-mercator.h"

#include "math.gl/web-mercator.h"

using namespace std;
using namespace mathgl;
using namespace deckgl;

Viewport::Viewport(const string& id, const ViewMatrixOptions& viewMatrixOptions,
                   const ProjectionMatrixOptions& projectionMatrixOptions, double x, double y, double width,
                   double height)
    : id{id}, x{x}, y{y}, width{width}, height{height} {
  // _initViewMatrix
  this->isGeospatial = viewMatrixOptions.isGeospatial;
  this->zoom = viewMatrixOptions.zoom;
  // elided: do not support default zoom
  this->scale = pow(2, this->zoom);

  auto lngLat = Vector2<double>(viewMatrixOptions.longitude, viewMatrixOptions.latitude);
  // TODO: doesn't support default distance scales
  this->distanceScales = this->isGeospatial ? getDistanceScales(lngLat) : viewMatrixOptions.distanceScales;
  this->focalDistance = viewMatrixOptions.focalDistance;

  // elided check for position defined
  this->position = viewMatrixOptions.position;
  this->modelMatrix = viewMatrixOptions.modelMatrix;
  // elided meterOffset

  if (this->isGeospatial) {
    this->longitude = viewMatrixOptions.longitude;
    this->latitude = viewMatrixOptions.latitude;
    this->center = this->_getCenterInWorld(lngLat);
  } else {
    // elided check for position defined
    this->center = this->projectPosition(position);
  }

  this->viewMatrixUncentered = viewMatrixOptions.viewMatrix;
  // Make a centered version of the matrix for projection modes without an offset
  // TODO: NEEDED
  // this->viewMatrix = Matrix4<double>()
  //                        // Apply the uncentered view matrix
  //                        .multiplyRight(this->viewMatrixUncentered)
  //                        // And center it
  //                        .translate(this->center)
  //                        .negate();
}

auto Viewport::metersPerPixel() -> double { return this->distanceScales.metersPerUnit.z / this->scale; }

auto Viewport::projectionMode() -> PROJECTION_MODE {
  if (this->isGeospatial) {
    return this->zoom < 12 ? PROJECTION_MODE::WEB_MERCATOR : PROJECTION_MODE::WEB_MERCATOR_AUTO_OFFSET;
  }
  return PROJECTION_MODE::IDENTITY;
}

auto Viewport::containsPixel(double x, double y, double width, double height) -> bool {
  return (x < this->x + this->width) && (this->x < x + width) && (y < this->y + this->height) && (this->y < y + height);
}

auto Viewport::projectFlat(const mathgl::Vector2<double>& xy) -> mathgl::Vector2<double> {
  if (this->isGeospatial) {
    return lngLatToWorld(xy);
  }
  return xy;
}

auto Viewport::unprojectFlat(const mathgl::Vector2<double>& xy) -> mathgl::Vector2<double> {
  if (this->isGeospatial) {
    return worldToLngLat(xy);
  }
  return xy;
}

auto Viewport::projectPosition(const mathgl::Vector2<double>& xy) -> mathgl::Vector2<double> {
  return this->projectFlat(xy);
}

auto Viewport::projectPosition(const mathgl::Vector3<double>& xyz) -> mathgl::Vector3<double> {
  auto projectedXy = this->projectFlat(xyz.toVector2());
  auto z = xyz.z * this->distanceScales.unitsPerMeter.z;
  return Vector3<double>(projectedXy, z);
}

auto Viewport::unprojectPosition(const mathgl::Vector2<double>& xy) -> mathgl::Vector2<double> {
  return this->unprojectFlat(xy);
}

auto Viewport::unprojectPosition(const mathgl::Vector3<double>& xyz) -> mathgl::Vector3<double> {
  auto unprojectedXy = this->unprojectFlat(xyz.toVector2());
  auto z = xyz.z * this->distanceScales.metersPerUnit.z;
  return Vector3<double>(unprojectedXy, z);
}

auto Viewport::getDistanceScales(const std::optional<mathgl::Vector2<double>>& coordinateOrigin)
    -> mathgl::DistanceScales {
  if (coordinateOrigin.has_value()) {
    return mathgl::getDistanceScales(coordinateOrigin.value(), true);
  }
  return this->distanceScales;
}

auto Viewport::getCameraPosition() -> mathgl::Vector3<double> { return this->cameraPosition; }

auto Viewport::getCameraDirection() -> mathgl::Vector3<double> { return this->cameraDirection; }

auto Viewport::getCameraUp() -> mathgl::Vector3<double> { return this->cameraUp; }

auto Viewport::_getCenterInWorld(const mathgl::Vector2<double>& lngLat) -> mathgl::Vector3<double> {
  // Make a centered version of the matrix for projection modes without an offset
  auto center2d = this->projectFlat(lngLat);
  auto center = Vector3<double>(center2d, 0);

  // elided
  // if (meterOffset) {
  //   const commonPosition = new Vector3(meterOffset)
  //     // Convert to pixels in current zoom
  //     .scale(distanceScales.unitsPerMeter);
  //   center.add(commonPosition);
  // }

  return center;
}

auto operator==(const Viewport& v1, const Viewport& v2) -> bool {
  return v1.width == v2.width && v1.height == v2.height && v1.scale == v2.scale &&
         v1.projectionMatrix == v2.projectionMatrix && v1.viewMatrix == v2.viewMatrix;
}

auto operator!=(const Viewport& v1, const Viewport& v2) -> bool { return !(v1 == v2); }
