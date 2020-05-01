// Copyright (c) 2020, Unfolded Inc
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

#ifndef DECKGL_CORE_SHADERLIB_PROJECT_VIEWPORTUNIFORMS_H
#define DECKGL_CORE_SHADERLIB_PROJECT_VIEWPORTUNIFORMS_H

#include <memory>
#include <optional>

#include "deck.gl/core.h"
#include "luma.gl/webgpu.h"
#include "math.gl/core.h"

namespace deckgl {

/// Uniform buffers use std140 memory layout, which makes vec3 16-byte aligned.
/// The order of fields in this structure is crucial for it to be mapped to its GLSL counterpart properly.
/// The current order is the most efficient representation of this structure. For more details on layout blocks, see:
/// https://learnopengl.com/Advanced-OpenGL/Advanced-GLSL
struct ViewportUniforms {
  mathgl::Matrix4<float> modelMatrix;
  mathgl::Matrix4<float> viewProjectionMatrix;
  mathgl::Vector4<float> center;
  alignas(16) mathgl::Vector3<float> commonUnitsPerMeter;
  int32_t coordinateSystem;
  alignas(16) mathgl::Vector3<float> commonUnitsPerWorldUnit;
  int32_t projectionMode;
  alignas(16) mathgl::Vector3<float> commonUnitsPerWorldUnit2;
  float scale;
  alignas(16) mathgl::Vector3<float> cameraPosition;
  float antimeridian;
  alignas(16) mathgl::Vector3<float> coordinateOrigin;
  float devicePixelRatio;
  mathgl::Vector2<float> viewportSize;
  float focalDistance;
  alignas(4) bool wrapLongitude;
};

/// \brief Returns uniforms for shaders based on current projection
/// \note Includes projection matrix suitable for shaders
/// \param viewport
/// \return 4x4 projection matrix that can be used in shaders
// TODO(issac@unfolded.ai): Ensure this works with any viewport, not just WebMercatorViewports
auto getUniformsFromViewport(const std::shared_ptr<Viewport>& viewport, double devicePixelRatio = 1,
                             mathgl::Matrix4<double> modelMatrix = mathgl::Matrix4<double>(),
                             COORDINATE_SYSTEM coordinateSystem = COORDINATE_SYSTEM::DEFAULT,
                             mathgl::Vector3<double> coordinateOrigin = mathgl::Vector3<double>(),
                             bool wrapLongitude = false) -> ViewportUniforms;

// TODO(ilija@unfolded.ai): Should this function be public? It provides incomplete uniforms
auto calculateViewportUniforms(const std::shared_ptr<Viewport>& viewport, double devicePixelRatio,
                               COORDINATE_SYSTEM coordinateSystem, mathgl::Vector3<double> coordinateOrigin)
    -> ViewportUniforms;

}  // namespace deckgl

#endif
