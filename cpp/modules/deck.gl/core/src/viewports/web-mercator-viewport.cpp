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

ViewMatrixOptions calculateViewMatrixOptions(const WebMercatorViewport::Options& opts) {
  auto scale = pow(2, opts.zoom);
  double adjustedHeight = opts.height == 0 ? 1 : opts.height;

  // The uncentered matrix allows us two move the center addition to the
  // shader (cheap) which gives a coordinate system that has its center in
  // the layer's center position. This makes rotations and other modelMatrx
  // transforms much more useful.
  auto viewMatrixUncentered = getViewMatrix(adjustedHeight, opts.pitch, opts.bearing, opts.altitude, scale);

  auto viewOpts = ViewMatrixOptions();
  viewOpts.viewMatrix = viewMatrixUncentered;
  viewOpts.isGeospatial = true;  // TODO(isaac@unfolded.ai): ???
  viewOpts.longitude = opts.longitude;
  viewOpts.latitude = opts.latitude;
  viewOpts.zoom = opts.zoom;
  viewOpts.position = Vector3<double>();
  // viewOpts.modelMatrix // TODO(isaac@unfolded.ai):
  // viewOpts.distanceScales // TODO(isaac@unfolded.ai):
  return viewOpts;
}

ProjectionMatrixOptions calculateProjectionMatrixOptions(const WebMercatorViewport::Options& opts) {
  double adjustedWidth = opts.width == 0 ? 1 : opts.width;
  double adjustedHeight = opts.height == 0 ? 1 : opts.height;

  // Altitude - prevent division by 0
  // TODO(isaac@unfolded.ai): - just throw an Error instead?
  double adjustedAltitude = max(0.75, opts.altitude);

  return getProjectionParameters(adjustedWidth, adjustedHeight, adjustedAltitude, opts.pitch, opts.nearZMultiplier,
                                 opts.farZMultiplier);
}

WebMercatorViewport::WebMercatorViewport(const WebMercatorViewport::Options& opts)
    : Viewport("web-mercator-viewport", calculateViewMatrixOptions(opts), calculateProjectionMatrixOptions(opts), 0, 0,
               opts.width == 0 ? 1 : opts.width, opts.height == 0 ? 1 : opts.height) {
  // TODO(isaac@unfolded.ai): Need to cleanup the call to the superclass ctor. In JS this is done partway through this
  // ctor, but that can't be done in C++. Perhaps just call a non-virtual _init method instead?

  // TODO(isaac@unfolded.ai):
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

auto WebMercatorViewport::fitBounds(mathgl::Vector2<double> topLeft, mathgl::Vector2<double> bottomRight, int padding,
                                    mathgl::Vector2<int> offset) -> WebMercatorViewport {
  // TODO(randy@unfolded.ai)
  // https://github.com/uber-web/math.gl/blob/master/modules/web-mercator/src/fit-bounds.js

  WebMercatorViewport::Options options;
  options.width = this->width;
  options.height = this->height;
  options.longitude = 0;
  options.latitude = 0;
  options.zoom = 0;

  // Assume constant padding for now
  // Call WebMercatorViewport constructor, passing in opts
  WebMercatorViewport viewport = WebMercatorViewport(options);

  auto nw = viewport.projectFlat(topLeft);
  auto se = viewport.projectFlat(bottomRight);

  // get abs of x/y difference
  // max(abs, minExtent)?
  const double size[] = {abs(se.x - nw.x), abs(se.y - nw.y)};

  const double targetSize[] = {this->width - 2 * padding - abs(offset.x) * 2,
                               this->height - 2 * padding - abs(offset.y) * 2};

  // mimicing assert(); in JS
  if (targetSize[0] <= 0 || targetSize[1] <= 0) {
    throw new std::logic_error("Invalid targetSize");
  }

  const double scaleX = targetSize[0] / size[0];
  const double scaleY = targetSize[1] / size[1];

  // From JS:
  //    Find how much we need to shift the center
  //    const offsetX = (padding.right - padding.left) / 2 / scaleX;
  //    const offsetY = (padding.bottom - padding.top) / 2 / scaleY;
  // offsetX and offsetY should return 0 because paddings are equal, remove from center calcualtion
  //    const center = [(se[0] + nw[0]) / 2 + offsetX, (se[1] + nw[1]) / 2 + offsetY];

  const mathgl::Vector2<double> center((se.x + nw.x) / 2, (se.y + nw.y) / 2);

  const mathgl::Vector2<double> centerLngLat = viewport.unproject(center);

  // maxZoom not present, no use for min(maxZoom, etc..)
  const auto zoom = this->zoom + log2(abs(min(scaleX, scaleY)));

  viewport.longitude = centerLngLat.x;
  viewport.latitude = centerLngLat.y;
  viewport.zoom = zoom;

  return viewport;

  // throw new std::logic_error("fitBounds not supported");
}
