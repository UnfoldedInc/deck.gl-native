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

#ifndef DECKGL_LAYERS_LINE_LAYER_FRAGMENT_H
#define DECKGL_LAYERS_LINE_LAYER_FRAGMENT_H

#include <string>

#include "deck.gl/core/src/shaderlib/misc/geometry.glsl.h"

namespace {

// NOLINTNEXTLINE(runtime/string)
static const std::string lineLayerFS = R"GLSL(
layout(location = 0) in vec4 vColor;
layout(location = 1) in vec2 uv;

layout(location = 0) out vec4 fragColor;

void main(void) {
  geometry.uv = uv;

  fragColor = vColor;
}
)GLSL";

}  // anonymous namespace

// NOLINTNEXTLINE(runtime/string)
static const std::string fs = "#version 450\n" + geometryFS + "\n" + lineLayerFS;

#endif  // DECKGL_LAYERS_LINE_LAYER_FRAGMENT_H
