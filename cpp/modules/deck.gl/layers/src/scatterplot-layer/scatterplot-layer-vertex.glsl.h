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

#ifndef DECKGL_LAYERS_SCATTERPLOT_LAYER_VERTEX_H
#define DECKGL_LAYERS_SCATTERPLOT_LAYER_VERTEX_H

#include <string>

#include "deck.gl/core/src/shaderlib/project/project32.glsl.h"
#include "deck.gl/core/src/shaderlib/misc/geometry.glsl.h"

namespace {

// NOLINTNEXTLINE(runtime/string)
static const std::string scatterplotLayerVS = R"GLSL(
layout(std140, set = 0, binding = 1) uniform ScatterplotLayerOptions {
  float opacity;
  float radiusScale;
  float radiusMinPixels;
  float radiusMaxPixels;
  float lineWidthScale;
  float lineWidthMinPixels;
  float lineWidthMaxPixels;
  float stroked;
  bool filled;
} layerOptions;

layout(location = 0) in vec3 positions;

layout(location = 1) in vec3 instancePositions;
layout(location = 2) in float instanceRadius;
layout(location = 3) in vec4 instanceFillColors;
layout(location = 4) in vec4 instanceLineColors;
layout(location = 5) in float instanceLineWidths;

// TODO(ilija@unfolded.ai): Revisit once double splitting is in place
vec3 instancePositions64Low = vec3(0.);

layout(location = 0) out vec4 vFillColor;
layout(location = 1) out vec4 vLineColor;
layout(location = 2) out vec2 unitPosition;
layout(location = 3) out float innerUnitRadius;
layout(location = 4) out float outerRadiusPixels;

void main(void) {
  geometry.worldPosition = instancePositions;

  // Multiply out radius and clamp to limits
  outerRadiusPixels = clamp(
    project_size_to_pixel(layerOptions.radiusScale * instanceRadius),
    layerOptions.radiusMinPixels, layerOptions.radiusMaxPixels
  );

  // Multiply out line width and clamp to limits
  float lineWidthPixels = clamp(
    project_size_to_pixel(layerOptions.lineWidthScale * instanceLineWidths),
    layerOptions.lineWidthMinPixels, layerOptions.lineWidthMaxPixels
  );

  // Outer radius needs to offset by half stroke width
  outerRadiusPixels += layerOptions.stroked * lineWidthPixels / 2.0;

  // Position on the containing square in [-1, 1] space
  unitPosition = positions.xy;
  geometry.uv = unitPosition;

  innerUnitRadius = 1.0 - layerOptions.stroked * lineWidthPixels / outerRadiusPixels;

  vec3 offset = positions * project_pixel_size(outerRadiusPixels);
  gl_Position = project_position_to_clipspace(instancePositions, instancePositions64Low, offset, geometry.position);

  // Apply opacity to instance color, or return instance picking color, then normalize the values
  vFillColor = clamp(vec4(instanceFillColors.rgb, instanceFillColors.a * layerOptions.opacity), 0, 255) / 255.0;
  vLineColor = clamp(vec4(instanceLineColors.rgb, instanceLineColors.a * layerOptions.opacity), 0, 255) / 255.0;
}
)GLSL";

}  // anonymous namespace

// NOLINTNEXTLINE(runtime/string)
static const std::string vs = "#version 450\n" + geometryVS + "\n" + project32VS + "\n" + scatterplotLayerVS;

#endif  // DECKGL_LAYERS_SCATTERPLOT_LAYER_VERTEX_H
