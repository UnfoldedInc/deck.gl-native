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

static const char* projectVS = R"GLSL(
#version 450

const int COORDINATE_SYSTEM_DEFAULT = -1;
const int COORDINATE_SYSTEM_LNGLAT = 1;
const int COORDINATE_SYSTEM_METER_OFFSETS = 2;
const int COORDINATE_SYSTEM_LNGLAT_OFFSETS = 3;
const int COORDINATE_SYSTEM_CARTESIAN = 0;

const int PROJECTION_MODE_WEB_MERCATOR = 1;
const int PROJECTION_MODE_WEB_MERCATOR_AUTO_OFFSET = 4;
const int PROJECTION_MODE_IDENTITY = 0;

layout(std140, set = 0, binding = 1) uniform ProjectOptions {
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
    PI + log(1.0 * 0.5)
    // error: 'tan_fp32' : no matching overloaded function found
    //PI + log(tan_fp32(PI * 0.25 + radians(lnglat.y) * 0.5))
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

static const char* vs = R"GLSL(
layout(std140, set = 0, binding = 0) uniform LineLayerOptions {
  float opacity;
  float widthScale;
  float widthMinPixels;
  float widthMaxPixels;
} layerOptions;

layout(location = 0) in vec3 positions;
layout(location = 1) in vec3 instanceSourcePositions;
layout(location = 2) in vec3 instanceSourcePositions64Low;
layout(location = 3) in vec3 instanceTargetPositions;
layout(location = 4) in vec3 instanceTargetPositions64Low;
layout(location = 5) in vec4 instanceColors;
layout(location = 6) in float instanceWidths;

layout(location = 0) out vec4 vColor;
layout(location = 1) out vec2 uv;

// offset vector by strokeWidth pixels
// offset_direction is -1 (left) or 1 (right)
vec2 getExtrusionOffset(vec2 line_clipspace, float offset_direction, float width) {
  // normalized direction of the line
  vec2 dir_screenspace = normalize(line_clipspace * project.uViewportSize);
  // rotate by 90 degrees
  dir_screenspace = vec2(-dir_screenspace.y, dir_screenspace.x);

  return dir_screenspace * offset_direction * width / 2.0;
}

void main(void) {
  // Position
  vec4 source_commonspace;
  vec4 target_commonspace;
  vec4 source = project_position_to_clipspace(instanceSourcePositions, instanceSourcePositions64Low, vec3(0.), source_commonspace);
  vec4 target = project_position_to_clipspace(instanceTargetPositions, instanceTargetPositions64Low, vec3(0.), target_commonspace);

  // Multiply out width and clamp to limits
  float widthPixels = clamp(
    project_size_to_pixel(instanceWidths * layerOptions.widthScale),
    layerOptions.widthMinPixels, layerOptions.widthMaxPixels
  );

  // linear interpolation of source & target to pick right coord
  float segmentIndex = positions.x;
  vec4 p = mix(source, target, segmentIndex);
  uv = positions.xy;

  // extrude
  vec3 offset = vec3(
    getExtrusionOffset(target.xy - source.xy, positions.y, widthPixels),
    0.0);
  gl_Position = p + vec4(project_pixel_size_to_clipspace(offset.xy), 0.0, 0.0);

  // Color
  vColor = vec4(instanceColors.rgb, instanceColors.a * layerOptions.opacity);
}
)GLSL";
