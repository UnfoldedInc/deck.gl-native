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

#ifndef MATHGL_WEB_MERCATOR_H
#define MATHGL_WEB_MERCATOR_H

#include <cmath>
#include <optional>

#include "math.gl/core.h"

namespace mathgl {

const auto PI = M_PI;
const auto PI_4 = PI / 4.0;
const auto DEGREES_TO_RADIANS = PI / 180.0;
const auto RADIANS_TO_DEGREES = 180.0 / PI;

const auto TILE_SIZE = 512;

// Average circumference (40075 km equatorial, 40007 km meridional)
const auto EARTH_CIRCUMFERENCE = 40.03e6;

// Mapbox default altitude
const auto DEFAULT_ALTITUDE = 1.5;

struct DistanceScales {
  Vector3<double> metersPerUnit;
  Vector3<double> unitsPerMeter;

  Vector3<double> unitsPerDegree;
  Vector3<double> degreesPerUnit;

  // High precision values (optional)
  Vector3<double> unitsPerDegree2;
  Vector3<double> unitsPerMeter2;
};

struct ViewMatrixOptions {
  Matrix4<double> viewMatrix;
  bool isGeospatial;
  // Anchor: lng lat zoom makes viewport work w/ geospatial coordinate systems
  double longitude;
  double latitude;
  double zoom;
  // Anchor position offset (in meters for geospatial viewports)
  Vector3<double> position;
  // A model matrix to be applied to position, to match the layer props API
  Matrix4<double> modelMatrix;
  // Only needed for orthographic views
  double focalDistance;
  DistanceScales distanceScales;

  ViewMatrixOptions();
  explicit ViewMatrixOptions(Matrix4<double> viewMatrix);
};

struct ProjectionMatrixOptions {
  // Projection matrix
  std::optional<Matrix4<double>> projectionMatrix;

  // Projection matrix parameters, used if projectionMatrix not supplied
  bool orthographic;
  double fovyRadians;
  double fovy;
  double aspect;
  // Distance of near clipping plane
  double near;
  // Distance of far clipping plane
  double far;
  double focalDistance;

  ProjectionMatrixOptions();
  ProjectionMatrixOptions(double fov, double aspect, double focalDistance, double near, double far);
};

auto zoomToScale(double zoom) -> double;
auto scaleToZoom(double scale) -> double;

/**
 * Project [lng,lat] on sphere onto [x,y] on 512*512 Mercator Zoom 0 tile.
 * Performs the nonlinear part of the web mercator projection.
 * Remaining projection is done with 4x4 matrices which also handles
 * perspective.
 *
 * @param {Array} lngLat - [lng, lat] coordinates
 *   Specifies a point on the sphere to project onto the map.
 * @return {Array} [x,y] coordinates.
 */
auto lngLatToWorld(Vector2<double> lngLat) -> Vector2<double>;

/**
 * Unproject world point [x,y] on map onto {lat, lon} on sphere
 *
 * @param {number[]} xy - array with [x,y] members
 *  representing point on projected map plane
 * @return {number[]} - array with [x,y] of point on sphere.
 *   Has toArray method if you need a GeoJSON Array.
 *   Per cartographic tradition, lat and lon are specified as degrees.
 */
auto worldToLngLat(Vector2<double> xy) -> Vector2<double>;

// Returns the zoom level that gives a 1 meter pixel at a certain latitude
// 1 = C*cos(y)/2^z/TILE_SIZE = C*cos(y)/2^(z+9)
auto getMeterZoom(double latitude) -> double;

/**
 * Calculate distance scales in meters around current lat/lon, both for
 * degrees and pixels.
 * In mercator projection mode, the distance scales vary significantly
 * with latitude.
 */
auto getDistanceScales(Vector2<double> lngLat, bool highPrecision = false) -> DistanceScales;

/**
 * Offset a lng/lat position by meterOffset (northing, easting)
 */
auto addMetersToLngLat(Vector3<double> lngLatZ, Vector3<double> xyz) -> Vector3<double>;
auto addMetersToLngLat(Vector2<double> lngLat, Vector2<double> xy) -> Vector2<double>;

// ATTRIBUTION:
// view and projection matrix creation is intentionally kept compatible with
// mapbox-gl's implementation to ensure that seamless interoperation
// with mapbox and react-map-gl. See: https://github.com/mapbox/mapbox-gl-js

auto getViewMatrix(double height, double pitch, double bearing, double altitude, double scale,
                   Vector3<double> = Vector3<double>(0, 0, 0)) -> Matrix4<double>;

// PROJECTION MATRIX PARAMETERS
// Variable fov (in radians)
auto getProjectionParameters(double width, double height, double altitude = DEFAULT_ALTITUDE, double pitch = 0,
                             double nearZMultiplier = 1, double farZMultiplier = 1) -> ProjectionMatrixOptions;

// PROJECTION MATRIX: PROJECTS FROM CAMERA (VIEW) SPACE TO CLIPSPACE
// To match mapbox's z buffer:
// <= 0.28 - nearZMultiplier: 0.1, farZmultiplier: 1
// >= 0.29 - nearZMultiplier: 1 / height, farZMultiplier: 1.01

auto getProjectionMatrix(double width, double height, double pitch, double altitude, double nearZMultiplier,
                         double farZMultipler) -> Matrix4<double>;

auto transformVector(Matrix4<double>, Vector4<double>) -> Vector4<double>;

/**
 * Project flat coordinates to pixels on screen.
 *
 * @param {Array} xyz - flat coordinate on 512*512 Mercator Zoom 0 tile
 * @param {Matrix4} pixelProjectionMatrix - projection matrix
 * @return {Array} [x, y, depth] pixel coordinate on screen.
 */

auto worldToPixels(Vector3<double> xyz, Matrix4<double> pixelProjectionMatrix) -> Vector3<double>;
auto worldToPixels(Vector2<double> xy, Matrix4<double> pixelProjectionMatrix) -> Vector2<double>;

/**
 * Unproject pixels on screen to flat coordinates.
 *
 * @param {Array} xyz - pixel coordinate on screen.
 * @param {Matrix4} pixelUnprojectionMatrix - unprojection matrix
 * @param {Number} targetZ - if pixel coordinate does not have a 3rd component (depth),
 *    targetZ is used as the elevation plane to unproject onto
 * @return {Array} [x, y, Z] flat coordinates on 512*512 Mercator Zoom 0 tile.
 */
auto pixelsToWorld(Vector3<double> xyz, Matrix4<double> pixelUnprojectionMatrix, double targetZ = 0) -> Vector3<double>;
auto pixelsToWorld(Vector2<double> xy, Matrix4<double> pixelUnprojectionMatrix, double targetZ = 0) -> Vector2<double>;

}  // namespace mathgl

#endif
