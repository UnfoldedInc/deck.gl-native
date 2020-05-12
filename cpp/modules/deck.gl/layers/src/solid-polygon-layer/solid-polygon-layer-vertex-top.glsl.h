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

#ifndef DECKGL_LAYERS_SOLID_POLYGON_LAYER_VERTEX_TOP_H
#define DECKGL_LAYERS_SOLID_POLYGON_LAYER_VERTEX_TOP_H

#include <string>

#include "deck.gl/core/src/shaderlib/project/project32.glsl.h"
#include "deck.gl/core/src/shaderlib/misc/geometry.glsl.h"
#include "deck.gl/layers/src/solid-polygon-layer/solid-polygon-layer-vertex-main.glsl.h"

namespace {

// NOLINTNEXTLINE(runtime/string)
static const std::string solidPolygonLayerVST1 = R"GLSL(
  layout(location = 0) in vec3 instancePositions;
  layout(location = 1) in float instanceElevations;
  layout(location = 2) in vec4 instanceFillColors;
  layout(location = 3) in vec4 instanceLineColors;
  layout(location = 4) in vec3 instancePickingColors;
  vec3 instancePositions64Low = vec3(0.);

)GLSL";

// NOLINTNEXTLINE(runtime/string)
static const std::string solidPolygonLayerVST2 = R"GLSL(
  void main(void){
    PolygonProps props;
    props.positions = instancePositions;
    props.positions64Low = instancePositions64Low;
    props.elevations = instanceElevations;
    props.fillColors = instanceFillColors;
    props.lineColors = instanceLineColors;
    props.pickingColors = instancePickingColors;

    calculatePosition(props);
  }
)GLSL";

// NOLINTNEXTLINE(runtime/string)
static const std::string solidPolygonLayerVST = solidPolygonLayerVST1 + solidPolygonLayerVSM + solidPolygonLayerVST2;
// NOLINTNEXTLINE(runtime/string
static const std::string vst = "#version450\n" + geometryVS + "\n" + project32VS + "\n" + solidPolygonLayerVST;

}  // anonymous namespace

#endif  // DECKGL_LAYERS_SOLID_POLYGON_LAYER_VERTEX_TOP_H
