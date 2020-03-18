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

ViewMatrixOptions::ViewMatrixOptions()
    : viewMatrix{Matrix4<double>()},  // identity
      focalDistance{1} {}

ProjectionMatrixOptions::ProjectionMatrixOptions()
    : orthographic{false}, fovy{75}, near{0.1}, far{1000}, focalDistance{1} {}

ProjectionMatrixOptions::ProjectionMatrixOptions(double fov, double aspect, double focalDistance, double near,
                                                 double far)
    // TODO: suspicious reuse of the fovy/fov field here
    : fovy(fov), aspect(aspect), near(near), far(far), focalDistance(focalDistance) {}

auto zoomToScale(double zoom) -> double { return pow(2, zoom); }

auto scaleToZoom(double scale) -> double { return log2(scale); }

auto lngLatToWorld(Vector2<double> lngLat) -> Vector2<double> {
  if (lngLat.y < -90 || lngLat.y > 90) {
    throw std::logic_error("invalid latitude");
  }

  auto lambda2 = lngLat.x * DEGREES_TO_RADIANS;
  auto phi2 = lngLat.y * DEGREES_TO_RADIANS;
  auto x = (TILE_SIZE * (lambda2 + PI)) / (2 * PI);
  auto y = (TILE_SIZE * (PI + log(tan(PI_4 + phi2 * 0.5)))) / (2 * PI);
  return {x, y};
}

auto worldToLngLat(Vector2<double> xy) -> Vector2<double> {
  auto lambda2 = (xy.x / TILE_SIZE) * (2 * PI) - PI;
  auto phi2 = 2 * (atan(exp((xy.y / TILE_SIZE) * (2 * PI) - PI)) - PI_4);
  return {lambda2 * RADIANS_TO_DEGREES, phi2 * RADIANS_TO_DEGREES};
}

auto getMeterZoom(double latitude) -> double {
  auto latCosine = cos(latitude * DEGREES_TO_RADIANS);
  return scaleToZoom(EARTH_CIRCUMFERENCE * latCosine) - 9;
}

auto getDistanceScales(Vector2<double> latLng, bool highPrecision) -> DistanceScales {
  auto worldSize = TILE_SIZE;
  auto latCosine = cos(latLng.x * DEGREES_TO_RADIANS);

  /**
   * Number of pixels occupied by one degree longitude around current lat/lon:
     unitsPerDegreeX = d(lngLatToWorld([lng, lat])[0])/d(lng)
       = scale * TILE_SIZE * DEGREES_TO_RADIANS / (2 * PI)
     unitsPerDegreeY = d(lngLatToWorld([lng, lat])[1])/d(lat)
       = -scale * TILE_SIZE * DEGREES_TO_RADIANS / cos(lat * DEGREES_TO_RADIANS)  / (2 * PI)
   */
  auto unitsPerDegreeX = worldSize / 360.0;
  auto unitsPerDegreeY = unitsPerDegreeX / latCosine;

  /**
   * Number of pixels occupied by one meter around current lat/lon:
   */
  auto altUnitsPerMeter = worldSize / EARTH_CIRCUMFERENCE / latCosine;
  auto inverseAltUnitsPerMeter = 1.0 / altUnitsPerMeter;

  /**
   * LngLat: longitude -> east and latitude -> north (bottom left)
   * UTM meter offset: x -> east and y -> north (bottom left)
   * World space: x -> east and y -> south (top left)
   *
   * Y needs to be flipped when converting delta degree/meter to delta pixels
   */
  DistanceScales result;
  result.unitsPerMeter = Vector3(altUnitsPerMeter, altUnitsPerMeter, altUnitsPerMeter);
  result.metersPerUnit = Vector3(inverseAltUnitsPerMeter, inverseAltUnitsPerMeter, inverseAltUnitsPerMeter);

  result.unitsPerDegree = Vector3(unitsPerDegreeX, unitsPerDegreeY, altUnitsPerMeter);
  result.degreesPerUnit = Vector3(1.0 / unitsPerDegreeX, 1.0 / unitsPerDegreeY, 1.0 / altUnitsPerMeter);

  /**
   * Taylor series 2nd order for 1/latCosine
     f'(a) * (x - a)
       = d(1/cos(lat * DEGREES_TO_RADIANS))/d(lat) * dLat
       = DEGREES_TO_RADIANS * tan(lat * DEGREES_TO_RADIANS) / cos(lat * DEGREES_TO_RADIANS) * dLat
   */
  if (highPrecision) {
    auto latCosine2 = (DEGREES_TO_RADIANS * tan(latLng.x * DEGREES_TO_RADIANS)) / latCosine;
    auto unitsPerDegreeY2 = (unitsPerDegreeX * latCosine2) / 2.0;
    auto altUnitsPerDegree2 = (worldSize / EARTH_CIRCUMFERENCE) * latCosine2;
    auto altUnitsPerMeter2 = (altUnitsPerDegree2 / unitsPerDegreeY) * altUnitsPerMeter;

    result.unitsPerDegree2 = Vector3(0.0, unitsPerDegreeY2, altUnitsPerDegree2);
    result.unitsPerMeter2 = Vector3(altUnitsPerMeter2, 0.0, altUnitsPerMeter2);
  }

  // Main results, used for converting meters to latlng deltas and scaling offsets
  return result;
}

/**
 * Offset a lng/lat position by meterOffset (northing, easting)
 */
auto addMetersToLngLat(Vector3<double> lngLatZ, Vector3<double> xyz) -> Vector3<double> {
  auto distanceScales = getDistanceScales(lngLatZ.toVector2(), true);

  auto worldspace = lngLatToWorld(lngLatZ.toVector2());
  worldspace.x += xyz.x * (distanceScales.unitsPerMeter.x + distanceScales.unitsPerMeter2.x * xyz.y);
  worldspace.y += xyz.y * (distanceScales.unitsPerMeter.y + distanceScales.unitsPerMeter2.y * xyz.y);

  auto newLngLat = worldToLngLat(worldspace);
  auto newZ = lngLatZ.z + xyz.z;

  return Vector3(newLngLat.x, newLngLat.y, newZ);
}

auto addMetersToLngLat(Vector2<double> lngLat, Vector2<double> xy) -> Vector2<double> {
  auto distanceScales = getDistanceScales(lngLat, true);

  auto worldspace = lngLatToWorld(lngLat);
  worldspace.x += xy.x * (distanceScales.unitsPerMeter.x + distanceScales.unitsPerMeter2.x * xy.y);
  worldspace.y += xy.y * (distanceScales.unitsPerMeter.y + distanceScales.unitsPerMeter2.y * xy.y);

  auto newLngLat = worldToLngLat(worldspace);

  return newLngLat;
}

auto getViewMatrix(double height, double pitch, double bearing, double altitude, double scale, Vector3<double> center)
    -> Matrix4<double> {
  // VIEW MATRIX: PROJECTS MERCATOR WORLD COORDINATES
  // Note that mercator world coordinates typically need to be flipped
  //
  // Note: As usual, matrix operation orders should be read in reverse
  // since vectors will be multiplied from the right during transformation
  auto vm = Matrix4<double>();

  // Move camera to altitude (along the pitch & bearing direction)
  auto translation = Vector3<double>(0.0, 0.0, -altitude);
  vm = vm *
       Matrix4<double>::MakeTranslation(translation)
       // Rotate by bearing, and then by pitch (which tilts the view)
       * Matrix4<double>::MakeRotationX(-pitch * DEGREES_TO_RADIANS) *
       Matrix4<double>::MakeRotationY(bearing * DEGREES_TO_RADIANS);

  scale /= height;

  Vector3<double> scaleVector = Vector3<double>(scale, scale, scale);
  vm = vm.scale(scaleVector);

  auto centerTranslation = -center;
  vm = vm * Matrix4<double>::MakeTranslation(centerTranslation);

  return vm;
}

// PROJECTION MATRIX PARAMETERS
// Variable fov (in radians)
auto getProjectionParameters(double width, double height, double altitude, double pitch, double nearZMultiplier,
                             double farZMultiplier) -> ProjectionMatrixOptions {
  // Find the distance from the center point to the center top
  // in altitude units using law of sines.
  auto pitchRadians = pitch * DEGREES_TO_RADIANS;
  auto halfFov = atan(0.5 / altitude);
  auto topHalfSurfaceDistance = (sin(halfFov) * altitude) / sin(PI / 2.0 - pitchRadians - halfFov);

  // Calculate z value of the farthest fragment that should be rendered.
  auto farZ = cos(PI / 2.0 - pitchRadians) * topHalfSurfaceDistance + altitude;

  return ProjectionMatrixOptions(2.0 * halfFov, width / height, altitude, nearZMultiplier, farZ * farZMultiplier);
}

// PROJECTION MATRIX: PROJECTS FROM CAMERA (VIEW) SPACE TO CLIPSPACE
// To match mapbox's z buffer:
// <= 0.28 - nearZMultiplier: 0.1, farZmultiplier: 1
// >= 0.29 - nearZMultiplier: 1 / height, farZMultiplier: 1.01
auto getProjectionMatrix(double width, double height, double pitch, double altitude, double nearZMultiplier,
                         double farZMultipler) -> Matrix4<double> {
  auto params = getProjectionParameters(width, height, altitude, pitch, nearZMultiplier, farZMultipler);

  auto projectionMatrix = Matrix4<double>::makePerspective(params.fovy, params.aspect, params.near, params.far);

  return projectionMatrix;
}

// Transforms a vec4 with a projection matrix
auto transformVector(Matrix4<double> matrix, Vector4<double> vec) -> Vector4<double> {
  auto result = matrix.transform(vec);
  result *= 1.0 / result.w;
  return result;
}

/**
 * Project flat coordinates to pixels on screen.
 *
 * @param {Array} xyz - flat coordinate on 512*512 Mercator Zoom 0 tile
 * @param {Matrix4} pixelProjectionMatrix - projection matrix
 * @return {Array} [x, y, depth] pixel coordinate on screen.
 */
auto worldToPixels(Vector3<double> xyz, Matrix4<double> pixelProjectionMatrix) -> Vector3<double> {
  return transformVector(pixelProjectionMatrix, Vector4<double>(xyz, 1)).toVector3();
}

auto worldToPixels(Vector2<double> xy, Matrix4<double> pixelProjectionMatrix) -> Vector2<double> {
  return transformVector(pixelProjectionMatrix, Vector4<double>(Vector3<double>(xy, 0), 1)).toVector3().toVector2();
}

/**
 * Unproject pixels on screen to flat coordinates.
 *
 * @param {Array} xyz - pixel coordinate on screen.
 * @param {Matrix4} pixelUnprojectionMatrix - unprojection matrix
 * @param {Number} targetZ - if pixel coordinate does not have a 3rd component (depth),
 *    targetZ is used as the elevation plane to unproject onto
 * @return {Array} [x, y, Z] flat coordinates on 512*512 Mercator Zoom 0 tile.
 */
auto pixelsToWorld(Vector3<double> xyz, Matrix4<double> pixelUnprojectionMatrix, double targetZ) -> Vector3<double> {
  return transformVector(pixelUnprojectionMatrix, Vector4<double>(xyz, 1)).toVector3();
}

auto pixelsToWorld(Vector2<double> xy, Matrix4<double> pixelUnprojectionMatrix, double targetZ) -> Vector2<double> {
  // since we don't know the correct projected z value for the point,
  // unproject two points to get a line and then find the point on that line with z=0
  auto coord0 = transformVector(pixelUnprojectionMatrix, Vector4<double>(xy.x, xy.y, 0, 1));
  auto coord1 = transformVector(pixelUnprojectionMatrix, Vector4<double>(xy.x, xy.y, 1, 1));

  auto z0 = coord0.z;
  auto z1 = coord1.z;

  auto t = z0 == z1 ? 0.0 : (targetZ - z0) / (z1 - z0);

  auto coord02 = coord0.toVector3().toVector2();
  auto coord12 = coord1.toVector3().toVector2();
  return coord02.lerp(coord12, t);
}

}  // namespace mathgl
