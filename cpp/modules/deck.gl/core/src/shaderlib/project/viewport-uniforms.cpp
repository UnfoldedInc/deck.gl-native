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

#include "./viewport-uniforms.h"
#include <cmath>

using namespace mathgl;

namespace deckgl {

auto getOffsetOrigin(Viewport viewport, COORDINATE_SYSTEM coordinateSystem, mathgl::Vector3<double> coordinateOrigin)
    -> OffsetOrigin {
  auto shaderCoordinateOrigin = coordinateOrigin;
  auto geospatialOrigin = std::optional<Vector3<double>>();
  auto offsetMode = true;

  if (coordinateSystem == COORDINATE_SYSTEM::LNGLAT_OFFSETS || coordinateSystem == COORDINATE_SYSTEM::METER_OFFSETS) {
    geospatialOrigin = coordinateOrigin;
  } else if (viewport.isGeospatial) {
    geospatialOrigin = Vector3(round(viewport.longitude), round(viewport.latitude), 0.0);
  }

  switch (viewport.projectionMode()) {
    case PROJECTION_MODE::WEB_MERCATOR:
      if (coordinateSystem == COORDINATE_SYSTEM::LNGLAT || coordinateSystem == COORDINATE_SYSTEM::CARTESIAN) {
        offsetMode = false;
      }
      break;

    case PROJECTION_MODE::WEB_MERCATOR_AUTO_OFFSET:
      if (coordinateSystem == COORDINATE_SYSTEM::LNGLAT) {
        // viewport center in world space
        // TODO: This must not be null per original logic but unclear how to prove that here
        shaderCoordinateOrigin = geospatialOrigin.value();
      } else if (coordinateSystem == COORDINATE_SYSTEM::CARTESIAN) {
        // viewport center in common space
        shaderCoordinateOrigin = Vector3<double>(round(viewport.center.x), round(viewport.center.y), 0.0);
        // Geospatial origin (wgs84) must match shaderCoordinateOrigin (common)
        geospatialOrigin = viewport.unprojectPosition(shaderCoordinateOrigin);
      }
      break;

    case PROJECTION_MODE::IDENTITY:
      shaderCoordinateOrigin =
          Vector3<double>(round(viewport.position.x), round(viewport.position.y), round(viewport.position.z));
      break;

    default:
      // Unknown projection mode
      offsetMode = false;
  }

  return {
      .geospatialOrigin = geospatialOrigin, .shaderCoordinateOrigin = shaderCoordinateOrigin, .offsetMode = offsetMode};
}

// The code that utilizes Matrix4 does the same calculation as their mathgl::Matrix4<double> counterparts,
// has lower performance but provides error checking.
auto calculateMatrixAndOffset(Viewport viewport, COORDINATE_SYSTEM coordinateSystem,
                              mathgl::Vector3<double> coordinateOrigin) -> MatrixAndOffset {
  //     const {viewMatrixUncentered, projectionMatrix} = viewport;
  auto viewMatrix = viewport.viewMatrix;
  auto viewProjectionMatrix = viewport.viewProjectionMatrix;
  auto projectionCenter = Vector4<double>();
  auto cameraPosCommon = viewport.cameraPosition;

  auto offsetOrigin = getOffsetOrigin(viewport, coordinateSystem, coordinateOrigin);

  if (offsetOrigin.offsetMode) {
    // Calculate transformed projectionCenter (using 64 bit precision JS)
    // This is the key to offset mode precision
    // (avoids doing this addition in 32 bit precision in GLSL)
    auto positionCommonSpace3 =
        viewport.projectPosition(offsetOrigin.geospatialOrigin.value_or(offsetOrigin.shaderCoordinateOrigin));

    cameraPosCommon = cameraPosCommon - positionCommonSpace3;

    auto positionCommonSpace = Vector4<double>(positionCommonSpace3, 1);

    projectionCenter = viewProjectionMatrix.transform(positionCommonSpace);

    // Always apply uncentered projection matrix if available (shader adds center)
    // TODO
    // viewMatrix = viewMatrixUncentered || viewMatrix;

    // Zero out 4th coordinate ("after" model matrix) - avoids further translations
    // TODO
    // viewProjectionMatrix = mat4.multiply([], projectionMatrix, viewMatrix);
    // viewProjectionMatrix = mat4.multiply([], viewProjectionMatrix, VECTOR_TO_POINT_MATRIX);
  }

  return {.viewMatrix = viewMatrix,
          .viewProjectionMatrix = viewProjectionMatrix,
          .projectionCenter = projectionCenter,
          .cameraPosCommon = cameraPosCommon,
          .shaderCoordinateOrigin = offsetOrigin.shaderCoordinateOrigin,
          .geospatialOrigin = offsetOrigin.geospatialOrigin};
}

}  // namespace deckgl
