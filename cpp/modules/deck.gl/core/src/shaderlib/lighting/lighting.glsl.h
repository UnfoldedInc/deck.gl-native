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

#ifndef DECKGL_CORE_SHADERLIB_LIGHTING_LIGHTING_GLSL_H
#define DECKGL_CORE_SHADERLIB_LIGHTING_LIGHTING_GLSL_H

#include <string>

// NOLINTNEXTLINE(runtime/string)
static const std::string lighting = R"GLSL(
//#if (defined(SHADER_TYPE_FRAGMENT) && defined(LIGHTING_FRAGMENT)) || (defined(SHADER_TYPE_VERTEX) && defined(LIGHTING_VERTEX))

#define MAX_LIGHTS 3

struct AmbientLight {
 vec3 color;
};

struct PointLight {
 vec3 color;
 vec3 position;
 // Constant-Linear-Exponential
 vec3 attenuation;
};

struct DirectionalLight {
  vec3 color;
  vec3 direction;
};

// TODO(ilija@unfolded.ai): These should be uniforms
AmbientLight lighting_uAmbientLight;
PointLight lighting_uPointLight[MAX_LIGHTS];
DirectionalLight lighting_uDirectionalLight[MAX_LIGHTS];
int lighting_uPointLightCount;
int lighting_uDirectionalLightCount;

bool lighting_uEnabled;

float getPointLightAttenuation(PointLight pointLight, float distance) {
  return pointLight.attenuation.x
       + pointLight.attenuation.y * distance
       + pointLight.attenuation.z * distance * distance;
}

//#endif
)GLSL";

#endif  // DECKGL_CORE_SHADERLIB_LIGHTING_LIGHTING_GLSL_H
