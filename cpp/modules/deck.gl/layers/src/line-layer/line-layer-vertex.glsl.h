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

#ifndef DECKGL_LAYERS_LINE_LAYER_VERTEX_H
#define DECKGL_LAYERS_LINE_LAYER_VERTEX_H

#include "./project-vertex.glsl.h"

static const char* vs = R"GLSL(
layout(std140, set = 0, binding = 1) uniform LineLayerOptions {
  float opacity;
  float widthScale;
  float widthMinPixels;
  float widthMaxPixels;
} layerOptions;

layout(location = 0) in vec3 positions;
layout(location = 1) in vec3 instanceSourcePositions;
layout(location = 2) in vec3 instanceTargetPositions;
layout(location = 3) in vec4 instanceColors;
layout(location = 4) in float instanceWidths;

// TODO(ilija@unfolded.ai): Revisit once double splitting is in place
vec3 instanceSourcePositions64Low = vec3(0.0);
vec3 instanceTargetPositions64Low = vec3(0.0);

layout(location = 0) out vec4 vColor;
layout(location = 1) out vec2 uv;

// TODO(ilija@unfolded.ai): Debug code, remove
layout(location = 2) out mat4 uModelMatrix;
layout(location = 6) out mat4 uViewProjectionMatrix;
layout(location = 10) out vec4 uCenter;
layout(location = 11) out vec3 uCommonUnitsPerMeter;
layout(location = 12) out vec3 uCommonUnitsPerWorldUnit;
layout(location = 13) out vec3 uCommonUnitsPerWorldUnit2;
layout(location = 14) out vec3 uCameraPosition;
layout(location = 15) out vec3 uCoordinateOrigin;
layout(location = 16) out vec2 uViewportSize;
layout(location = 17) out int uCoordinateSystem;
layout(location = 18) out int uProjectionMode;
layout(location = 19) out float uScale;
layout(location = 20) out float uAntimeridian;
layout(location = 21) out float uDevicePixelRatio;
layout(location = 22) out float uFocalDistance;

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
  uCoordinateSystem = project.uCoordinateSystem;
  uProjectionMode = project.uProjectionMode;
  uScale = project.uScale;
  uAntimeridian = project.uAntimeridian;
  uCommonUnitsPerMeter = project.uCommonUnitsPerMeter;
  uCommonUnitsPerWorldUnit = project.uCommonUnitsPerWorldUnit;
  uCommonUnitsPerWorldUnit2 = project.uCommonUnitsPerWorldUnit2;
  uCenter = project.uCenter;
  uModelMatrix = project.uModelMatrix;
  uViewProjectionMatrix = project.uViewProjectionMatrix;
  uViewportSize = project.uViewportSize;
  uDevicePixelRatio = project.uDevicePixelRatio;
  uFocalDistance = project.uFocalDistance;
  uCameraPosition = project.uCameraPosition;
  uCoordinateOrigin = project.uCoordinateOrigin;

  // Position
  vec4 sourceCommonspace;
  vec4 targetCommonspace;
  vec4 source = project_position_to_clipspace(instanceSourcePositions, instanceSourcePositions64Low, vec3(0.), sourceCommonspace);
  vec4 target = project_position_to_clipspace(instanceTargetPositions, instanceTargetPositions64Low, vec3(0.), targetCommonspace);

  // Multiply out width and clamp to limits
  float widthPixels = clamp(
    project_size_to_pixel(instanceWidths * layerOptions.widthScale),
    layerOptions.widthMinPixels, layerOptions.widthMaxPixels
  );

  // Linear interpolation of source & target to pick right coord
  float segmentIndex = positions.x;
  vec4 p = mix(source, target, segmentIndex);
  uv = positions.xy;

  // Extrude
  vec3 offset = vec3(getExtrusionOffset(target.xy - source.xy, positions.y, widthPixels), 0.0);
  gl_Position = p + vec4(project_pixel_size_to_clipspace(offset.xy), 0.0, 0.0);

  // Color
  vColor = vec4(instanceColors.rgb, instanceColors.a * layerOptions.opacity);
}
)GLSL";

#endif  // DECKGL_LAYERS_LINE_LAYER_VERTEX_H
