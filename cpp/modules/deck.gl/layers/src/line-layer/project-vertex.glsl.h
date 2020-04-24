// Copyright (c) 2020 Unfolded, Inc.
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

#ifndef DECKGL_LAYERS_PROJECT_VERTEX_H
#define DECKGL_LAYERS_PROJECT_VERTEX_H

static const char* projectVS = R"GLSL(
#version 450

// TODO(ilija@unfolded.ai): Port from luna.gl math32 module
float tan_fp32(float radians) {
  return tan(radians);
}

const int COORDINATE_SYSTEM_DEFAULT = -1;
const int COORDINATE_SYSTEM_LNGLAT = 1;
const int COORDINATE_SYSTEM_METER_OFFSETS = 2;
const int COORDINATE_SYSTEM_LNGLAT_OFFSETS = 3;
const int COORDINATE_SYSTEM_CARTESIAN = 0;

const int PROJECTION_MODE_WEB_MERCATOR = 1;
const int PROJECTION_MODE_WEB_MERCATOR_AUTO_OFFSET = 4;
const int PROJECTION_MODE_IDENTITY = 0;

layout(std140, set = 0, binding = 0) uniform ProjectOptions {
  int uCoordinateSystem;
  int uProjectionMode;
  float uScale;
  bool uWrapLongitude;
  float uAntimeridian;
  vec3 uCommonUnitsPerMeter;
  vec3 uCommonUnitsPerWorldUnit;
  vec3 uCommonUnitsPerWorldUnit2;
  vec4 uCenter;
  mat4 uModelMatrix;
  mat4 uViewProjectionMatrix;
  vec2 uViewportSize;
  float uDevicePixelRatio;
  float uFocalDistance;
  vec3 uCameraPosition;
  vec3 uCoordinateOrigin;
} project;

const float TILE_SIZE = 512.0;
const float PI = 3.1415926536;
const float WORLD_SCALE = TILE_SIZE / (PI * 2.0);
const vec3 ZERO_64_LOW = vec3(0.0);

//
// Scaling offsets - scales meters to "world distance"
// Note the scalar version of project_size is for scaling the z component only
//
float project_size(float meters) {
  return meters * project.uCommonUnitsPerMeter.z;
}

vec2 project_size(vec2 meters) {
  return meters * project.uCommonUnitsPerMeter.xy;
}

vec3 project_size(vec3 meters) {
  return meters * project.uCommonUnitsPerMeter;
}

vec4 project_size(vec4 meters) {
  return vec4(meters.xyz * project.uCommonUnitsPerMeter, meters.w);
}

//
// Projecting normal - transform deltas from current coordinate system to
// normals in the worldspace
//
vec3 project_normal(vec3 vector) {
  // Apply model matrix
  vec4 normal_modelspace = project.uModelMatrix * vec4(vector, 0.0);
  return normalize(normal_modelspace.xyz * project.uCommonUnitsPerMeter);
}

vec4 project_offset_(vec4 offset) {
  float dy = offset.y;
  if (project.uCoordinateSystem == COORDINATE_SYSTEM_LNGLAT) {
    dy = clamp(dy, -1., 1.);
  }
  vec3 commonUnitsPerWorldUnit = project.uCommonUnitsPerWorldUnit + project.uCommonUnitsPerWorldUnit2 * dy;
  return vec4(offset.xyz * commonUnitsPerWorldUnit, offset.w);
}

//
// Projecting positions - non-linear projection: lnglats => unit tile [0-1, 0-1]
//
vec2 project_mercator_(vec2 lnglat) {
  float x = lnglat.x;
  if (project.uWrapLongitude) {
    x = mod(x - project.uAntimeridian, 360.0) + project.uAntimeridian;
  }
  return vec2(
    radians(x) + PI,
    PI + log(tan_fp32(PI * 0.25 + radians(lnglat.y) * 0.5))
  );
}

//
// Projects positions (defined by project_uCoordinateSystem) to common space (defined by project_uProjectionMode)
//
vec4 project_position(vec4 position, vec3 position64Low) {
  vec4 position_world = project.uModelMatrix * position;

  // Work around for a Mac+NVIDIA bug https://github.com/uber/deck.gl/issues/4145
  if (project.uProjectionMode == PROJECTION_MODE_WEB_MERCATOR) {
    if (project.uCoordinateSystem == COORDINATE_SYSTEM_LNGLAT) {
      return vec4(
        project_mercator_(position_world.xy) * WORLD_SCALE,
        project_size(position_world.z),
        position_world.w
      );
    }
  }
  if (project.uProjectionMode == PROJECTION_MODE_WEB_MERCATOR_AUTO_OFFSET &&
    (project.uCoordinateSystem == COORDINATE_SYSTEM_LNGLAT ||
     project.uCoordinateSystem == COORDINATE_SYSTEM_CARTESIAN)) {
    // Subtract high part of 64 bit value. Convert remainder to float32, preserving precision.
    position_world.xyz -= project.uCoordinateOrigin;
    position_world.xyz += position64Low;
  }
  if (project.uProjectionMode == PROJECTION_MODE_IDENTITY) {
    position_world.xyz -= project.uCoordinateOrigin;
    // Translation is already added to the high parts
    position_world += project.uModelMatrix * vec4(position64Low, 0.0);
  }

  return project_offset_(position_world);
}

vec4 project_position(vec4 position) {
  return project_position(position, ZERO_64_LOW);
}

vec3 project_position(vec3 position, vec3 position64Low) {
  vec4 projected_position = project_position(vec4(position, 1.0), position64Low);
  return projected_position.xyz;
}

vec3 project_position(vec3 position) {
  vec4 projected_position = project_position(vec4(position, 1.0), ZERO_64_LOW);
  return projected_position.xyz;
}

vec2 project_position(vec2 position) {
  vec4 projected_position = project_position(vec4(position, 0.0, 1.0), ZERO_64_LOW);
  return projected_position.xy;
}

vec4 project_common_position_to_clipspace(vec4 position, mat4 viewProjectionMatrix, vec4 center) {
  return viewProjectionMatrix * position + center;
}

//
// Projects from common space coordinates to clip space.
// Uses project_uViewProjectionMatrix
//
vec4 project_common_position_to_clipspace(vec4 position) {
  return project_common_position_to_clipspace(position, project.uViewProjectionMatrix, project.uCenter);
}

// Returns a clip space offset that corresponds to a given number of screen pixels
vec2 project_pixel_size_to_clipspace(vec2 pixels) {
  vec2 offset = pixels / project.uViewportSize * project.uDevicePixelRatio * 2.0;
  return offset * project.uFocalDistance;
}

float project_size_to_pixel(float meters) {
  return project_size(meters) * project.uScale;
}
float project_pixel_size(float pixels) {
  return pixels / project.uScale;
}
vec2 project_pixel_size(vec2 pixels) {
  return pixels / project.uScale;
}

/* PROJECT32 */

vec4 project_position_to_clipspace(
  vec3 position, vec3 position64Low, vec3 offset, out vec4 commonPosition
) {
  vec3 projectedPosition = project_position(position, position64Low);
  commonPosition = vec4(projectedPosition + offset, 1.0);
  return project_common_position_to_clipspace(commonPosition);
}

vec4 project_position_to_clipspace(
  vec3 position, vec3 position64Low, vec3 offset
) {
  vec4 commonPosition;
  return project_position_to_clipspace(position, position64Low, offset, commonPosition);
}

/* PROJECT32 */
)GLSL";

#endif  // DECKGL_LAYERS_PROJECT_VERTEX_H
