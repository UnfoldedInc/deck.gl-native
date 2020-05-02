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

#ifndef DECKGL_LAYERS_SCATTERPLOT_LAYER_FRAGMENT_H
#define DECKGL_LAYERS_SCATTERPLOT_LAYER_FRAGMENT_H

#include <string>

#include "deck.gl/core/src/shaderlib/misc/geometry.glsl.h"

namespace {

// NOLINTNEXTLINE(runtime/string)
static const std::string scatterplotLayerFS = R"GLSL(
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

layout(location = 0) in vec4 vFillColor;
layout(location = 1) in vec4 vLineColor;
layout(location = 2) in vec2 unitPosition;
layout(location = 3) in float innerUnitRadius;
layout(location = 4) in float outerRadiusPixels;

layout(location = 0) out vec4 fragColor;

void main(void) {
  geometry.uv = unitPosition;

  float distToCenter = length(unitPosition) * outerRadiusPixels;
  float inCircle = smoothedge(distToCenter, outerRadiusPixels);

  if (inCircle == 0.0) {
    discard;
  }

  if (layerOptions.stroked > 0.5) {
    float isLine = smoothedge(innerUnitRadius * outerRadiusPixels, distToCenter);
    if (layerOptions.filled) {
      fragColor = mix(vFillColor, vLineColor, isLine);
    } else {
      if (isLine == 0.0) {
        discard;
      }
      fragColor = vec4(vLineColor.rgb, vLineColor.a * isLine);
    }
  } else if (layerOptions.filled) {
    fragColor = vFillColor;
  } else {
    discard;
  }

  fragColor.a *= inCircle;
}
)GLSL";

}  // anonymous namespace

// NOLINTNEXTLINE(runtime/string)
static const std::string fs = "#version 450\n" + geometryFS + "\n" + scatterplotLayerFS;

#endif  // DECKGL_LAYERS_SCATTERPLOT_LAYER_FRAGMENT_H
