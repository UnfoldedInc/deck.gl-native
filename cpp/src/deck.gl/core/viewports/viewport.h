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

#ifndef DECKGL_CORE_VIEWPORTS_VIEWPORT_H
#define DECKGL_CORE_VIEWPORTS_VIEWPORT_H

#include <string>

#include "deck.gl/core/lib/constants.h"
#include "math.gl/core.h"
#include "math.gl/web-mercator/web-mercator-utils.h"

namespace deckgl {

class Viewport {
 public:
  std::string id;
  double x;
  double y;
  double width;
  double height;
  double _frustumPlanes;  // TODO: actually an object
  bool isGeospatial;
  double zoom;
  double scale;
  mathgl::DistanceScales distanceScales;
  double focalDistance;
  mathgl::Vector3d position;
  mathgl::Vector3d meterOffset;
  mathgl::Matrix4d modelMatrix;
  double longitude;
  double latitude;
  mathgl::Vector3d center;
  mathgl::Matrix4d viewMatrixUncentered;
  mathgl::Matrix4d viewMatrix;

  // projectionProps in JS
  bool projectionOrthographic;
  double projectionFovyRadians;
  double projectionAspect;
  double projectionFocalDistance;
  double projectionNear;
  double projectionFar;

  mathgl::Matrix4d projectionMatrix;
  mathgl::Matrix4d viewProjectionMatrix;
  mathgl::Matrix4d viewMatrixInverse;
  mathgl::Vector3d cameraPosition;
  mathgl::Vector3d cameraDirection;
  mathgl::Vector3d cameraUp;
  mathgl::Vector3d cameraRight;

  mathgl::Matrix4d pixelProjectionMatrix;
  mathgl::Matrix4d viewportMatrix;
  mathgl::Matrix4d pixelUnrpojectionMatrix;

  Viewport(const std::string& id, const mathgl::ViewMatrixOptions& viewMatrixOptions,
           const mathgl::ProjectionMatrixOptions& projectionMatrixOptions,
           // Window width/height in pixels (for pixel projection)
           double x = 0, double y = 0, double width = 1, double height = 1);

  auto metersPerPixel() -> double;
  auto projectionMode() -> PROJECTION_MODE;

  /**
   * Projects xyz (possibly latitude and longitude) to pixel coordinates in
   * window using viewport projection parameters
   * - [longitude, latitude] to [x, y]
   * - [longitude, latitude, Z] => [x, y, z]
   * Note: By default, returns top-left coordinates for canvas/SVG type render
   *
   * @param {Array} lngLatZ - [lng, lat] or [lng, lat, Z]
   * @param {Object} opts - options
   * @param {Object} opts.topLeft=true - Whether projected coords are top left
   * @return {Array} - [x, y] or [x, y, z] in top left coords
   */
  auto project(const mathgl::Vector2d& latlng, bool topLeft = true) -> mathgl::Vector2d;
  auto project(const mathgl::Vector3d& latlng, bool topLeft = true) -> mathgl::Vector3d;

  /**
   * Unproject pixel coordinates on screen onto world coordinates,
   * (possibly [lon, lat]) on map.
   * - [x, y] => [lng, lat]
   * - [x, y, z] => [lng, lat, Z]
   * @param {Array} xyz -
   * @param {Object} opts - options
   * @param {Object} opts.topLeft=true - Whether origin is top left
   * @return {Array|null} - [lng, lat, Z] or [X, Y, Z]
   */
  auto unproject(const mathgl::Vector2d& xy, bool topLeft = true) -> mathgl::Vector2d;
  auto unproject(const mathgl::Vector3d& xy, bool topLeft = true) -> mathgl::Vector3d;

  // NON_LINEAR PROJECTION HOOKS
  // Used for web meractor projection

  auto projectPosition(const mathgl::Vector2d& xy) -> mathgl::Vector2d;
  auto projectPosition(const mathgl::Vector3d& xyz) -> mathgl::Vector3d;

  auto unprojectPosition(const mathgl::Vector2d& xy) -> mathgl::Vector2d;
  auto unprojectPosition(const mathgl::Vector3d& xyz) -> mathgl::Vector3d;

  /**
   * Project [lng,lat] on sphere onto [x,y] on 512*512 Mercator Zoom 0 tile.
   * Performs the nonlinear part of the web mercator projection.
   * Remaining projection is done with 4x4 matrices which also handles
   * perspective.
   * @param {Array} lngLat - [lng, lat] coordinates
   *   Specifies a point on the sphere to project onto the map.
   * @return {Array} [x,y] coordinates.
   */
  auto projectFlat(const mathgl::Vector2d& xy) -> mathgl::Vector2d;
  auto projectFlat(const mathgl::Vector3d& xyz) -> mathgl::Vector3d;

  /**
   * Unproject world point [x,y] on map onto {lat, lon} on sphere
   * @param {object|mathgl::Vector} xy - object with {x,y} members
   *  representing point on projected map plane
   * @return {GeoCoordinates} - object with {lat,lon} of point on sphere.
   *   Has toArray method if you need a GeoJSON Array.
   *   Per cartographic tradition, lat and lon are specified as degrees.
   */
  auto unprojectFlat(const mathgl::Vector2d& xy) -> mathgl::Vector2d;
  auto unprojectFlat(const mathgl::Vector3d& xyz) -> mathgl::Vector3d;

  auto getDistancceScales(const mathgl::Vector2d* coordinateOrigin) -> double;
  auto containsPixel(double x, double y, double width = 1, double height = 1) -> bool;

  // Extract frustum planes in common space
  // TODO: don't know type
  void getFrustrumPlanes();

  // EXPERIMENTAL METHODS

  auto getCameraPosition() -> mathgl::Vector3d;
  auto getCameraDirection() -> mathgl::Vector3d;
  auto getCameraUp() -> mathgl::Vector3d;

 private:
  auto _createProjectionMatrix(bool orthographic, double fovyRadians, double aspect, double focalDistance, double near,
                               double far) -> mathgl::Matrix4d;

  void _initViewMatrix(/* opts */);

  auto _getCenterInWorld(double longitude, double latitude) -> mathgl::Vector3d;

  void _initProjectionMatrix(/* opts */);

  void _initPixelMatrices();
};

} // namespace deckgl

auto operator==(const deckgl::Viewport& v1, const deckgl::Viewport& v2) -> bool;
auto operator!=(const deckgl::Viewport& v1, const deckgl::Viewport& v2) -> bool;

#endif
