#ifndef MATHGL_WEB_MERCATOR_H
#define MATHGL_WEB_MERCATOR_H

#include <cmath>
#include "math.gl/core.h"

namespace mathgl {

#define PI M_PI
#define PI_4 (M_PI / 4.0)
#define DEGREES_TO_RADIANS (PI / 180.0)
#define RADIANS_TO_DEGREES (180.0 / PI)
#define TILE_SIZE 512
// Average circumference (40075 km equatorial, 40007 km meridional)
#define EARTH_CIRCUMFERENCE 40.03e6

// Mapbox default altitude
#define DEFAULT_ALTITUDE 1.5

struct DistanceScales {
  Vector3d metersPerUnit;
  Vector3d unitsPerMeter;
};

struct ViewMatrixOptions {
  mathgl::Matrix4d viewMatrix;
  // Anchor: lng lat zoom makes viewport work w/ geospatial coordinate systems
  double longitude;
  double latitude;
  double zoom;
  // Anchor position offset (in meters for geospatial viewports)
  mathgl::Vector3d position;
  // A model matrix to be applied to position, to match the layer props API
  mathgl::Matrix4d modelMatrix;
  // Only needed for orthographic views
  double focalDistance;
  mathgl::DistanceScales distanceScales;

  ViewMatrixOptions();
};

struct ProjectionMatrixOptions {
  // Projection matrix
  mathgl::Matrix4d projectionMatrix;

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
auto lngLatToWorld(Vector2d lngLat) -> Vector2d;

/**
 * Unproject world point [x,y] on map onto {lat, lon} on sphere
 *
 * @param {number[]} xy - array with [x,y] members
 *  representing point on projected map plane
 * @return {number[]} - array with [x,y] of point on sphere.
 *   Has toArray method if you need a GeoJSON Array.
 *   Per cartographic tradition, lat and lon are specified as degrees.
 */
auto worldToLngLat(Vector2d xy) -> Vector2d;

// Returns the zoom level that gives a 1 meter pixel at a certain latitude
// 1 = C*cos(y)/2^z/TILE_SIZE = C*cos(y)/2^(z+9)
auto getMeterZoom(double latitude) -> double;

/**
 * Calculate distance scales in meters around current lat/lon, both for
 * degrees and pixels.
 * In mercator projection mode, the distance scales vary significantly
 * with latitude.
 */
auto getDistanceScales(Vector2d latLng, bool highPrecision = false) -> DistanceScales;

/**
 * Offset a lng/lat position by meterOffset (northing, easting)
 */
auto addMetersToLngLat(Vector3d lngLatZ) -> Vector3d;
auto addMetersToLngLat(Vector2d lngLat) -> Vector2d;

// ATTRIBUTION:
// view and projection matrix creation is intentionally kept compatible with
// mapbox-gl's implementation to ensure that seamless interoperation
// with mapbox and react-map-gl. See: https://github.com/mapbox/mapbox-gl-js

auto getViewMatrix(double height, double pitch, double bearing, double altitude, double scale, Vector3d center)
    -> Matrix4d;

// PROJECTION MATRIX PARAMETERS
// Variable fov (in radians)
auto getProjectionParameters(double width, double height, double altitude = DEFAULT_ALTITUDE, double pitch = 0,
                             double nearZMultiplier = 1, double farZMultiplier = 1) -> ProjectionMatrixOptions;

// PROJECTION MATRIX: PROJECTS FROM CAMERA (VIEW) SPACE TO CLIPSPACE
// To match mapbox's z buffer:
// <= 0.28 - nearZMultiplier: 0.1, farZmultiplier: 1
// >= 0.29 - nearZMultiplier: 1 / height, farZMultiplier: 1.01

auto getProjectionMatrix(double width, double height, double pitch, double altitude, double nearZMultiplier,
                         double farZMultipler) -> Matrix4d;

/**
 * Project flat coordinates to pixels on screen.
 *
 * @param {Array} xyz - flat coordinate on 512*512 Mercator Zoom 0 tile
 * @param {Matrix4} pixelProjectionMatrix - projection matrix
 * @return {Array} [x, y, depth] pixel coordinate on screen.
 */
auto worldToPixels(Vector3d xyz, Matrix4d pixelProjectionMatrix) -> Vector3d;
auto worldToPixels(Vector2d xy, Matrix4d pixelProjectionMatrix) -> Vector2d;

/**
 * Unproject pixels on screen to flat coordinates.
 *
 * @param {Array} xyz - pixel coordinate on screen.
 * @param {Matrix4} pixelUnprojectionMatrix - unprojection matrix
 * @param {Number} targetZ - if pixel coordinate does not have a 3rd component (depth),
 *    targetZ is used as the elevation plane to unproject onto
 * @return {Array} [x, y, Z] flat coordinates on 512*512 Mercator Zoom 0 tile.
 */
auto pixelsToWorld(Vector3d xyz, Matrix4d pixelUnprojectionMatrix, double targetZ) -> Vector3d;
auto pixelsToWorld(Vector2d xy, Matrix4d pixelUnprojectionMatrix, double targetZ) -> Vector2d;

}  // namespace mathgl

#endif
