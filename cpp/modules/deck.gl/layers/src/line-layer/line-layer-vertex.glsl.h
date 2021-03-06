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

#include <string>

#include "deck.gl/core/src/shaderlib/project/project32.glsl.h"
#include "deck.gl/core/src/shaderlib/misc/geometry.glsl.h"

namespace {

// NOLINTNEXTLINE(runtime/string)
static const std::string lineLayerVS = R"GLSL(
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
vec3 instanceSourcePositions64Low = vec3(0.);
vec3 instanceTargetPositions64Low = vec3(0.);

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
  geometry.worldPosition = instanceSourcePositions;
  geometry.worldPositionAlt = instanceTargetPositions;

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
  geometry.position = mix(sourceCommonspace, targetCommonspace, segmentIndex);
  uv = positions.xy;
  geometry.uv = uv;

  // Extrude
  vec3 offset = vec3(getExtrusionOffset(target.xy - source.xy, positions.y, widthPixels), 0.0);
  gl_Position = p + vec4(project_pixel_size_to_clipspace(offset.xy), 0.0, 0.0);

  // Color
  vec4 normalizedInstanceColors = clamp(instanceColors, 0, 255) / 255.0;
  vColor = vec4(normalizedInstanceColors.rgb, normalizedInstanceColors.a * layerOptions.opacity);
}
)GLSL";

}  // anonymous namespace

// NOLINTNEXTLINE(runtime/string)
static const std::string vs = "#version 450\n" + geometryVS + "\n" + project32VS + "\n" + lineLayerVS;

#endif  // DECKGL_LAYERS_LINE_LAYER_VERTEX_H
