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

#ifndef DECKGL_CORE_SHADERLIB_PROJECT_PROJECT32_GLSL_H
#define DECKGL_CORE_SHADERLIB_PROJECT_PROJECT32_GLSL_H

#include <string>

#include "./project.glsl.h"

// NOLINTNEXTLINE(runtime/string)
static const std::string project32VS = projectVS + "\n" + R"GLSL(
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
)GLSL";

#endif  // DECKGL_CORE_SHADERLIB_PROJECT_PROJECT32_GLSL_H

