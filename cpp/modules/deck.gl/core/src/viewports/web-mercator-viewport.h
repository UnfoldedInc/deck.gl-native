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

#ifndef DECKGL_CORE_VIEWPORTS_WEB_MERCATOR_VIEWPORT_H
#define DECKGL_CORE_VIEWPORTS_WEB_MERCATOR_VIEWPORT_H

#include <cmath>

#include "./viewport.h"

namespace deckgl {

// View and Projection Matrix calculations for mapbox-js style map view properties

// Creates view/projection matrices from mercator params
// Note: The Viewport is immutable in the sense that it only has accessors.
// A new viewport instance should be created if any parameters have changed.
//
class WebMercatorViewport : public Viewport {
 public:
  struct Options {
    const int DEFAULT_WIDTH = 1;
    const int DEFAULT_HEIGHT = 1;
    const double DEFAULT_LONGITUDE = 0.0;
    const double DEFAULT_LATITUDE = 0.0;
    const double DEFAULT_ZOOM = 11.0;
    const double DEFAULT_PITCH = 0.0;
    const double DEFAULT_BEARING = 0.0;
    const double DEFAULT_ALTITUDE = 1.5;
    const double DEFAULT_NEAR_Z_MULTIPLIER = 0.1;
    const double DEFAULT_FAR_Z_MULTIPLIER = 1.01;
    const bool DEFAULT_ORTHOGRAPHIC = false;
    const bool DEFAULT_REPEAT = false;
    const double DEFAULT_WORLD_OFFSET = 0.0;

    int width = DEFAULT_WIDTH;
    int height = DEFAULT_HEIGHT;
    double latitude = DEFAULT_LATITUDE;
    double longitude = DEFAULT_LONGITUDE;
    double zoom = DEFAULT_ZOOM;
    double pitch = DEFAULT_PITCH;
    double bearing = DEFAULT_BEARING;
    double altitude = DEFAULT_ALTITUDE;
    double nearZMultiplier = DEFAULT_NEAR_Z_MULTIPLIER;
    double farZMultiplier = DEFAULT_FAR_Z_MULTIPLIER;
    bool orthographic = DEFAULT_ORTHOGRAPHIC;
    bool repeat = DEFAULT_REPEAT;
    double worldOffset = DEFAULT_WORLD_OFFSET;
  };

  WebMercatorViewport(const Options& options);

  // elided subViewports feature
  // get subViewports()

  /**
   * Add a meter delta to a base lnglat coordinate, returning a new lnglat array
   *
   * Note: Uses simple linear approximation around the viewport center
   * Error increases with size of offset (roughly 1% per 100km)
   *
   * @param {[Number,Number]|[Number,Number,Number]) lngLatZ - base coordinate
   * @param {[Number,Number]|[Number,Number,Number]) xyz - array of meter deltas
   * @return {[Number,Number]|[Number,Number,Number]) array of [lng,lat,z] deltas
   */
  auto addMetersToLngLat(mathgl::Vector3<double> lngLatZ, mathgl::Vector3<double> xyz) -> mathgl::Vector3<double>;
  auto addMetersToLngLat(mathgl::Vector2<double> lngLat, mathgl::Vector2<double> xy) -> mathgl::Vector2<double>;

  /**
   * Get the map center that place a given [lng, lat] coordinate at screen
   * point [x, y]
   *
   * @param {Array} lngLat - [lng,lat] coordinates
   *   Specifies a point on the sphere.
   * @param {Array} pos - [x,y] coordinates
   *   Specifies a point on the screen.
   * @return {Array} [lng,lat] new map center.
   */
  auto getMapCenterByLngLatPosition(mathgl::Vector2<double> lngLat, mathgl::Vector2<double> pos)
      -> mathgl::Vector2<double>;

  /**
   * Returns a new viewport that fit around the given rectangle.
   * Only supports non-perspective mode.
   * @param {Array} bounds - [[lon, lat], [lon, lat]]
   * @param {Number} [options.padding] - The amount of padding in pixels to add to the given bounds.
   * @param {Array} [options.offset] - The center of the given bounds relative to the map's center,
   *    [x, y] measured in pixels.
   * @returns {WebMercatorViewport}
   */
  auto fitBounds(mathgl::Vector2<double> topLeft, mathgl::Vector2<double> bottomRight, int padding = 0,
                 mathgl::Vector2<int> offset = mathgl::Vector2<int>());
};

}  // namespace deckgl

#endif  // DECKGL_CORE_VIEWPORTS_WEB_MERCATOR_VIEWPORT_H
