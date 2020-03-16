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
  auto distanceScales = getDistanceScales(lngLatZ.ToVector2(), true);

  auto worldspace = lngLatToWorld(lngLatZ.ToVector2());
  worldspace.x += xyz.x * (distanceScales.unitsPerMeter.x + distanceScales.unitsPerMeter2.x * xyz.x);
  worldspace.y += xyz.y * (distanceScales.unitsPerMeter.y + distanceScales.unitsPerMeter2.y * xyz.y);

  auto newLngLat = worldToLngLat(worldspace);
  auto newZ = lngLatZ.z + xyz.z;

  return Vector3(newLngLat.x, newLngLat.y, newZ);
}

auto addMetersToLngLat(Vector2<double> lngLat, Vector2<double> xy) -> Vector2<double> {
  auto distanceScales = getDistanceScales(lngLat, true);

  auto worldspace = lngLatToWorld(lngLat);
  worldspace.x += xy.x * (distanceScales.unitsPerMeter.x + distanceScales.unitsPerMeter2.x * xy.x);
  worldspace.y += xy.y * (distanceScales.unitsPerMeter.y + distanceScales.unitsPerMeter2.y * xy.y);

  auto newLngLat = worldToLngLat(worldspace);

  return newLngLat;
}

}  // namespace mathgl
