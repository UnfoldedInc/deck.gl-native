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

#include <string>
#include "lib/constants.h"
#include "math/math.h"

using namespace mathgl;
using namespace std;

class Viewport {
  Viewport();
  // Viewport(/* opts */);

  double metersPerPixel();
  PROJECTION_MODE projectionMode();

  bool operator==(const Viewport& other);
  bool operator!=(const Viewport& other);

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
  Vector2d project(const Vector2d& latlng, bool topLeft = true);
  Vector3d project(const Vector3d& latlng, bool topLeft = true);

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
  Vector2d unproject(const Vector2d& xy, bool topLeft = true);
  Vector3d unproject(const Vector3d& xy, bool topLeft = true);

  // NON_LINEAR PROJECTION HOOKS
  // Used for web meractor projection

  Vector2d projectPosition(const Vector2d& xy);
  Vector3d projectPosition(const Vector3d& xyz);

  Vector2d unprojectPosition(const Vector2d& xy);
  Vector3d unprojectPosition(const Vector3d& xyz);

  /**
   * Project [lng,lat] on sphere onto [x,y] on 512*512 Mercator Zoom 0 tile.
   * Performs the nonlinear part of the web mercator projection.
   * Remaining projection is done with 4x4 matrices which also handles
   * perspective.
   * @param {Array} lngLat - [lng, lat] coordinates
   *   Specifies a point on the sphere to project onto the map.
   * @return {Array} [x,y] coordinates.
   */
  Vector2d projectFlat(const Vector2d& xy);
  Vector3d projectFlat(const Vector3d& xyz);

  /**
   * Unproject world point [x,y] on map onto {lat, lon} on sphere
   * @param {object|Vector} xy - object with {x,y} members
   *  representing point on projected map plane
   * @return {GeoCoordinates} - object with {lat,lon} of point on sphere.
   *   Has toArray method if you need a GeoJSON Array.
   *   Per cartographic tradition, lat and lon are specified as degrees.
   */
  Vector2d unprojectFlat(const Vector2d& xy);
  Vector3d unprojectFlat(const Vector3d& xyz);

  double getDistancceScales(const Vector2d* coordinateOrigin);
  bool containsPixel(double x, double y, double width = 1, double height = 1);

  // Extract frustum planes in common space
  // TODO: don't know type
  void getFrustrumPlanes();

  // EXPERIMENTAL METHODS

  Vector3d getCameraPosition();
  Vector3d getCameraDirection();
  Vector3d getCameraUp();

  // INTERNAL METHODS

 private:
  string id;
  double x;
  double y;
  double width;
  double height;
  double _frustumPlanes;  // TODO: actually an object
  bool isGeospatial;
  double zoom;
  double scale;
  Vector3d distanceScales;  // TODO: actually two Vector3d
  double focalDistance;
  Vector3d position;
  Vector3d meterOffset;
  Vector3d position;
  Matrix4d modelMatrix;
  Matrix4d meterOffset;
  double longitude;
  double latitude;
  Vector3d center;
  Matrix4d viewMatrixUncentered;
  Matrix4d viewMatrix;

  // projectionProps in JS
  bool projectionOrthographic;
  double projectionFovyRadians;
  double projectionAspect;
  double projectionFocalDistance;
  double projectionNear;
  double projectionFar;

  Matrix4d porjectionMatrix;
  Matrix4d viewProjectionMatrix;
  Matrix4d viewMatrixInverse;
  Vector3d cameraPosition;
  Vector3d cameraDirection;
  Vector3d cameraUp;
  Vector3d cameraRight;

  Matrix4d pixelProjectionMatrix;
  Matrix4d viewportMatrix;
  Matrix4d pixelUnrpojectionMatrix;

  // Private methods

  Matrix4d _createProjectionMatrix(bool orthographic, double fovyRadians,
                                   double aspect, double focalDistance,
                                   double near, double far);

  void _initViewMatrix(/* opts */);

  Vector3d _getCenterInWorld(double longitude, double latitude);

  void _initProjectionMatrix(/* opts */);

  void _initPixelMatrices();
}
