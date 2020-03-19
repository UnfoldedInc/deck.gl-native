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

#ifndef DECKGL_CORE_SHADERLIB_PROJECT_VIEWPORTUNIFORMS_H
#define DECKGL_CORE_SHADERLIB_PROJECT_VIEWPORTUNIFORMS_H

#include <optional>
#include "deck.gl/core.h"
#include "math.gl/core.h"

namespace deckgl {

struct ViewportUniforms {
  COORDINATE_SYSTEM project_uCoordinateSystem;
  PROJECTION_MODE project_uProjectionMode;
  mathgl::Vector3<double> project_uCoordinateOrigin;
  mathgl::Vector3<double> project_uCenter;
  double project_uAntimeridian;
  mathgl::Vector2<double> project_uViewportSize;
  double project_uDevicePixelRatio;
  double project_uFocalDistance;
  mathgl::Vector3<double> project_uCommonUnitsPerMeter;
  mathgl::Vector3<double> project_uCommonUnitsPerWorldUnit;
  mathgl::Vector3<double> project_uCommonUnitsPerWorldUnit2;
  double project_uScale;
  mathgl::Matrix4<double> project_uViewProjectionMatrix;
  mathgl::Vector3<double> project_uCameraPosition;

  bool project_uWrapLongitude;
  mathgl::Matrix4<double> project_uModelMatrix;
};

// TODO: consider finding another way rather than these structs. Perhaps make the functions
// that build them constructors?
struct OffsetOrigin {
  // TODO: temporarily replicating the JS logic of this possibly being null
  std::optional<mathgl::Vector3<double>> geospatialOrigin;
  mathgl::Vector3<double> shaderCoordinateOrigin;
  bool offsetMode;
};

struct MatrixAndOffset {
  mathgl::Matrix4<double> viewMatrix;
  mathgl::Matrix4<double> viewProjectionMatrix;
  mathgl::Vector4<double> projectionCenter;
  mathgl::Vector3<double> cameraPosCommon;
  mathgl::Vector3<double> shaderCoordinateOrigin;
  std::optional<mathgl::Vector3<double>> geospatialOrigin;
};

auto getOffsetOrigin(Viewport viewport, COORDINATE_SYSTEM coordinateSystem, mathgl::Vector3<double> coordinateOrigin)
    -> OffsetOrigin;

// The code that utilizes Matrix4 does the same calculation as their mathgl::Matrix4<double> counterparts,
// has lower performance but provides error checking.
auto calculateMatrixAndOffset(Viewport viewport, COORDINATE_SYSTEM coordinateSystem,
                              mathgl::Vector3<double> coordinateOrigin) -> MatrixAndOffset;

/**
 * Returns uniforms for shaders based on current projection
 * includes: projection matrix suitable for shaders
 *
 * TODO - Ensure this works with any viewport, not just WebMercatorViewports
 *
 * @param {WebMercatorViewport} viewport -
 * @return {Float32Array} - 4x4 projection matrix that can be used in shaders
 */
auto getUniformsFromViewport(Viewport, double devicePixelRatio = 1,
                             mathgl::Matrix4<double> modelMatrix = mathgl::Matrix4<double>(),
                             COORDINATE_SYSTEM coordinateSystem = COORDINATE_SYSTEM::DEFAULT,
                             mathgl::Vector3<double> coordinateOrigin = mathgl::Vector3<double>(),
                             bool wrapLongitude = false
                             // deprecated projectionMode, positionOrigin
                             ) -> ViewportUniforms;

auto calculateViewportUniforms(Viewport, double devicePixelRatio, COORDINATE_SYSTEM coordinateSystem,
                               mathgl::Vector3<double> coordinateOrigin) -> ViewportUniforms;

}  // namespace deckgl

#endif
