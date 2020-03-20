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

#include <optional>
#include <string>

#include "../lib/constants.h"
#include "math.gl/core.h"
#include "math.gl/web-mercator.h"

namespace deckgl {

class Viewport {
 public:
  std::string id;
  double x;
  double y;
  double width;
  double height;
  // Not needed
  double _frustumPlanes;  // TODO: actually an object
  // NEEDED
  bool isGeospatial;
  double zoom;
  // NEEDED
  double scale;
  // NEEDED
  mathgl::DistanceScales distanceScales;
  // NEEDED
  double focalDistance;
  // NEEDED
  mathgl::Vector3<double> position;
  mathgl::Vector3<double> meterOffset;
  mathgl::Matrix4<double> modelMatrix;
  // NEEDED
  double longitude;
  // NEEDED
  double latitude;
  // NEEDED
  mathgl::Vector3<double> center;
  mathgl::Matrix4<double> viewMatrixUncentered;
  mathgl::Matrix4<double> viewMatrix;

  // projectionProps in JS
  bool projectionOrthographic;
  double projectionFovyRadians;
  double projectionAspect;
  double projectionFocalDistance;
  double projectionNear;
  double projectionFar;

  mathgl::Matrix4<double> projectionMatrix;
  mathgl::Matrix4<double> viewProjectionMatrix;
  mathgl::Matrix4<double> viewMatrixInverse;
  // NEEDED
  mathgl::Vector3<double> cameraPosition;
  mathgl::Vector3<double> cameraDirection;
  mathgl::Vector3<double> cameraUp;
  mathgl::Vector3<double> cameraRight;

  mathgl::Matrix4<double> pixelProjectionMatrix;
  mathgl::Matrix4<double> viewportMatrix;
  mathgl::Matrix4<double> pixelUnrpojectionMatrix;

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
  auto project(const mathgl::Vector2<double>& latlng, bool topLeft = true) -> mathgl::Vector2<double>;
  auto project(const mathgl::Vector3<double>& latlng, bool topLeft = true) -> mathgl::Vector3<double>;

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
  auto unproject(const mathgl::Vector2<double>& xy, bool topLeft = true) -> mathgl::Vector2<double>;
  auto unproject(const mathgl::Vector3<double>& xy, bool topLeft = true) -> mathgl::Vector3<double>;

  // NON_LINEAR PROJECTION HOOKS
  // Used for web meractor projection

  auto projectPosition(const mathgl::Vector2<double>& xy) -> mathgl::Vector2<double>;
  auto projectPosition(const mathgl::Vector3<double>& xyz) -> mathgl::Vector3<double>;

  auto unprojectPosition(const mathgl::Vector2<double>& xy) -> mathgl::Vector2<double>;
  auto unprojectPosition(const mathgl::Vector3<double>& xyz) -> mathgl::Vector3<double>;

  /**
   * Project [lng,lat] on sphere onto [x,y] on 512*512 Mercator Zoom 0 tile.
   * Performs the nonlinear part of the web mercator projection.
   * Remaining projection is done with 4x4 matrices which also handles
   * perspective.
   * @param {Array} lngLat - [lng, lat] coordinates
   *   Specifies a point on the sphere to project onto the map.
   * @return {Array} [x,y] coordinates.
   */
  auto projectFlat(const mathgl::Vector2<double>& xy) -> mathgl::Vector2<double>;

  /**
   * Unproject world point [x,y] on map onto {lat, lon} on sphere
   * @param {object|mathgl::Vector} xy - object with {x,y} members
   *  representing point on projected map plane
   * @return {GeoCoordinates} - object with {lat,lon} of point on sphere.
   *   Has toArray method if you need a GeoJSON Array.
   *   Per cartographic tradition, lat and lon are specified as degrees.
   */
  auto unprojectFlat(const mathgl::Vector2<double>& xy) -> mathgl::Vector2<double>;

  auto getDistanceScales(const std::optional<mathgl::Vector2<double>>& coordinateOrigin =
                             std::optional<mathgl::Vector2<double>>()) -> mathgl::DistanceScales;
  auto containsPixel(double x, double y, double width = 1, double height = 1) -> bool;

  // Extract frustum planes in common space
  // TODO: don't know type
  void getFrustrumPlanes();

  // EXPERIMENTAL METHODS

  auto getCameraPosition() -> mathgl::Vector3<double>;
  auto getCameraDirection() -> mathgl::Vector3<double>;
  auto getCameraUp() -> mathgl::Vector3<double>;

 private:
  auto _createProjectionMatrix(bool orthographic, double fovyRadians, double aspect, double focalDistance, double near,
                               double far) -> mathgl::Matrix4<double>;

  void _initViewMatrix(const mathgl::ViewMatrixOptions& viewMatrixOptions);

  auto _getCenterInWorld(const mathgl::Vector2<double>& lngLat) -> mathgl::Vector3<double>;

  void _initProjectionMatrix(const mathgl::ProjectionMatrixOptions& projectionMatrixOptions);

  void _initPixelMatrices();
};

}  // namespace deckgl

auto operator==(const deckgl::Viewport& v1, const deckgl::Viewport& v2) -> bool;
auto operator!=(const deckgl::Viewport& v1, const deckgl::Viewport& v2) -> bool;

#endif
