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

#include "./viewport.h"  // NOLINT(build/include)

#include "math.gl/web-mercator.h"

using namespace std;
using namespace mathgl;
using namespace deckgl;

Viewport::Viewport(const string& id, const ViewMatrixOptions& viewMatrixOptions,
                   const ProjectionMatrixOptions& projectionMatrixOptions, double x, double y, double width,
                   double height)
    : id{id}, x{x}, y{y}, width{width}, height{height} {
  this->_initViewMatrix(viewMatrixOptions);
  this->_initProjectionMatrix(projectionMatrixOptions);
  this->_initPixelMatrices();
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

auto Viewport::unproject(const mathgl::Vector2<double>& xy, bool topLeft, double targetZ) -> mathgl::Vector2<double> {
  auto y2 = topLeft ? xy.y : this->height - xy.y;
  auto targetZWorld = targetZ * this->distanceScales.unitsPerMeter.z;
  auto coord = pixelsToWorld(Vector2<double>(xy.x, y2), this->pixelUnprojectionMatrix, targetZWorld);
  // elided: returning the targetZ if the user provided it
  return this->unprojectPosition(coord);
}

auto Viewport::unproject(const mathgl::Vector3<double>& xyz, bool topLeft, double targetZ) -> mathgl::Vector3<double> {
  auto y2 = topLeft ? xyz.y : this->height - xyz.y;
  auto targetZWorld = targetZ * this->distanceScales.unitsPerMeter.z;
  auto coord = pixelsToWorld(Vector3<double>(xyz.x, y2, xyz.z), this->pixelUnprojectionMatrix, targetZWorld);
  return this->unprojectPosition(coord);
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

auto Viewport::_createProjectionMatrix(bool orthographic, double fovyRadians, double aspect, double focalDistance,
                                       double near, double far) -> mathgl::Matrix4<double> {
  // TODO(isaac@unfolded.ai): support orthographic
  return orthographic ? throw new std::logic_error("orthographic not supported")
                      : Matrix4<double>::makePerspective(fovyRadians, aspect, near, far);
}

void Viewport::_initViewMatrix(const ViewMatrixOptions& viewMatrixOptions) {
  this->isGeospatial = viewMatrixOptions.isGeospatial;
  this->zoom = viewMatrixOptions.zoom;
  // elided: do not support default zoom
  this->scale = pow(2, this->zoom);

  auto lngLat = Vector2<double>(viewMatrixOptions.longitude, viewMatrixOptions.latitude);
  // TODO(isaac@unfolded.ai): doesn't support default distance scales
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
  this->viewMatrix = (Matrix4<double>::MakeUnit() * this->viewMatrixUncentered).translate(-this->center);
}

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

void Viewport::_initProjectionMatrix(const ProjectionMatrixOptions& opts) {
  this->projectionMatrix = opts.projectionMatrix.has_value()
                               ? opts.projectionMatrix.value()
                               : this->_createProjectionMatrix(opts.orthographic, opts.fovy, opts.aspect,
                                                               opts.focalDistance, opts.near, opts.far);
}

void Viewport::_initPixelMatrices() {
  // Note: As usual, matrix operations should be applied in "reverse" order
  // since vectors will be multiplied in from the right during transformation
  auto vpm = Matrix4<double>::MakeUnit();
  vpm = vpm * this->projectionMatrix;
  vpm = vpm * this->viewMatrix;

  // Flip the view projection matrix on Z-axis in order to make it WebGPU-compatible
  //  vpm = vpm * Matrix4<double>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 0.0, 0.0, 0.0, 1.0};

  this->viewProjectionMatrix = vpm;

  /*
  // Calculate inverse view matrix
  this.viewMatrixInverse = mat4.invert([], this.viewMatrix) || this.viewMatrix;

  // Decompose camera directions
  const {eye, direction, up, right} = extractCameraVectors({
    viewMatrix: this.viewMatrix,
    viewMatrixInverse: this.viewMatrixInverse
  });
  this.cameraPosition = eye;
  this.cameraDirection = direction;
  this.cameraUp = up;
  this.cameraRight = right;

  // console.log(this.cameraPosition, this.cameraDirection, this.cameraUp);
  */

  /*
   * Builds matrices that converts preprojected lngLats to screen pixels
   * and vice versa.
   * Note: Currently returns bottom-left coordinates!
   * Note: Starts with the GL projection matrix and adds steps to the
   *       scale and translate that matrix onto the window.
   * Note: WebGL controls clip space to screen projection with gl.viewport
   *       and does not need this step.
   */

  /*
  // matrix for conversion from world location to screen (pixel) coordinates
  const viewportMatrix = createMat4(); // matrix from NDC to viewport.
  const pixelProjectionMatrix = createMat4(); // matrix from world space to viewport.
  mat4.scale(viewportMatrix, viewportMatrix, [this.width / 2, -this.height / 2, 1]);
  mat4.translate(viewportMatrix, viewportMatrix, [1, -1, 0]);
  mat4.multiply(pixelProjectionMatrix, viewportMatrix, this.viewProjectionMatrix);
  this.pixelProjectionMatrix = pixelProjectionMatrix;
  this.viewportMatrix = viewportMatrix;

  this.pixelUnprojectionMatrix = mat4.invert(createMat4(), this.pixelProjectionMatrix);
  if (!this.pixelUnprojectionMatrix) {
    log.warn('Pixel project matrix not invertible')();
    // throw new Error('Pixel project matrix not invertible');
  }
  */
}

auto operator==(const Viewport& v1, const Viewport& v2) -> bool {
  return v1.width == v2.width && v1.height == v2.height && v1.scale == v2.scale &&
         v1.projectionMatrix == v2.projectionMatrix && v1.viewMatrix == v2.viewMatrix;
}

auto operator!=(const Viewport& v1, const Viewport& v2) -> bool { return !(v1 == v2); }
