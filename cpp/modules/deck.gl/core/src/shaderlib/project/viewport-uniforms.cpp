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

#include "./viewport-uniforms.h"  // NOLINT(build/include)

#include <cmath>

using namespace mathgl;

namespace deckgl {

auto const VECTOR_TO_POINT_MATRIX = Matrix4<double>(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0);
auto const DEFAULT_PIXELS_PER_UNIT2 = Vector3<double>();

// TODO(isaac): consider finding another way rather than these structs. Perhaps make the functions
// that build them constructors?
struct OffsetOrigin {
  // TODO(isaac): temporarily replicating the JS logic of this possibly being null
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
        // TODO(isaac): This must not be null per original logic but unclear how to prove that here
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
  auto viewMatrixUncentered = viewport.viewMatrixUncentered;
  auto viewMatrix = viewport.viewMatrix;
  auto viewProjectionMatrix = viewport.viewProjectionMatrix;
  auto projectionMatrix = viewport.projectionMatrix;
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
    // TODO(isaac): elided default of viewport.viewMatrix
    viewMatrix = viewMatrixUncentered;

    // Zero out 4th coordinate ("after" model matrix) - avoids further translations
    viewProjectionMatrix = projectionMatrix * viewMatrix;
    viewProjectionMatrix = viewProjectionMatrix * VECTOR_TO_POINT_MATRIX;
  }

  return {.viewMatrix = viewMatrix,
          .viewProjectionMatrix = viewProjectionMatrix,
          .projectionCenter = projectionCenter,
          .cameraPosCommon = cameraPosCommon,
          .shaderCoordinateOrigin = offsetOrigin.shaderCoordinateOrigin,
          .geospatialOrigin = offsetOrigin.geospatialOrigin};
}

auto getUniformsFromViewport(Viewport viewport, double devicePixelRatio, mathgl::Matrix4<double> modelMatrix,
                             COORDINATE_SYSTEM coordinateSystem, mathgl::Vector3<double> coordinateOrigin,
                             bool wrapLongitude) -> ViewportUniforms {
  if (coordinateSystem == COORDINATE_SYSTEM::DEFAULT) {
    coordinateSystem = viewport.isGeospatial ? COORDINATE_SYSTEM::LNGLAT : COORDINATE_SYSTEM::CARTESIAN;
  }

  // Memoized in the JS
  auto uniforms = calculateViewportUniforms(viewport, devicePixelRatio, coordinateSystem, coordinateOrigin);

  uniforms.project_uWrapLongitude = wrapLongitude;
  // elided IDENTITY_MATRIX default
  uniforms.project_uModelMatrix = viewport.modelMatrix;

  return uniforms;
}

auto calculateViewportUniforms(Viewport viewport, double devicePixelRatio, COORDINATE_SYSTEM coordinateSystem,
                               mathgl::Vector3<double> coordinateOrigin) -> ViewportUniforms {
  auto matrixAndOffset = calculateMatrixAndOffset(viewport, coordinateSystem, coordinateOrigin);

  // Calculate projection pixels per unit
  auto distanceScales = viewport.getDistanceScales();

  auto viewportSize = Vector2<double>(viewport.width, viewport.height) * devicePixelRatio;

  ViewportUniforms uniforms = {// Projection mode values
                               .project_uCoordinateSystem = coordinateSystem,
                               .project_uProjectionMode = viewport.projectionMode(),
                               .project_uCoordinateOrigin = matrixAndOffset.shaderCoordinateOrigin,
                               .project_uCenter = matrixAndOffset.projectionCenter,
                               .project_uAntimeridian = (viewport.isGeospatial ? viewport.longitude : 0) - 180,

                               // Screen size
                               .project_uViewportSize = viewportSize,
                               .project_uDevicePixelRatio = devicePixelRatio,

                               // Distance at which screen pixels are projected
                               // TODO(isaac): optional focalDistance
                               .project_uFocalDistance = viewport.focalDistance,
                               .project_uCommonUnitsPerMeter = distanceScales.unitsPerMeter,
                               .project_uCommonUnitsPerWorldUnit = distanceScales.unitsPerMeter,
                               .project_uCommonUnitsPerWorldUnit2 = DEFAULT_PIXELS_PER_UNIT2,
                               .project_uScale = viewport.scale,  // This is the mercator scale (2 ** zoom)

                               .project_uViewProjectionMatrix = matrixAndOffset.viewProjectionMatrix,

                               // This is for lighting calculations
                               .project_uCameraPosition = matrixAndOffset.cameraPosCommon};

  if (matrixAndOffset.geospatialOrigin.has_value()) {
    auto distanceScalesAtOrigin = viewport.getDistanceScales(matrixAndOffset.geospatialOrigin.value().toVector2());
    switch (coordinateSystem) {
      case COORDINATE_SYSTEM::METER_OFFSETS:
        uniforms.project_uCommonUnitsPerWorldUnit = distanceScalesAtOrigin.unitsPerMeter;
        uniforms.project_uCommonUnitsPerWorldUnit2 = distanceScalesAtOrigin.unitsPerMeter2;
        break;

      case COORDINATE_SYSTEM::LNGLAT:
      case COORDINATE_SYSTEM::LNGLAT_OFFSETS:
        uniforms.project_uCommonUnitsPerWorldUnit = distanceScalesAtOrigin.unitsPerDegree;
        uniforms.project_uCommonUnitsPerWorldUnit2 = distanceScalesAtOrigin.unitsPerDegree2;
        break;

      // a.k.a "preprojected" positions
      case COORDINATE_SYSTEM::CARTESIAN:
        uniforms.project_uCommonUnitsPerWorldUnit = Vector3<double>(1, 1, distanceScalesAtOrigin.unitsPerMeter.z);
        uniforms.project_uCommonUnitsPerWorldUnit2 = Vector3<double>(0, 0, distanceScalesAtOrigin.unitsPerMeter2.z);
        break;

      default:
        break;
    }
  }

  return uniforms;
}

}  // namespace deckgl
