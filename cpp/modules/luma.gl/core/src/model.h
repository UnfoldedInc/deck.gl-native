// Copyright (c) 2020 Unfolded Inc.
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

#ifndef LUMAGL_CORE_MODEL_H
#define LUMAGL_CORE_MODEL_H

#include <dawn/webgpu_cpp.h>

#include <memory>
#include <string>

#include "luma.gl/webgpu.h"

namespace lumagl {

/// \brief Holds shaders compiled and linked into a pipeline
class Model {
 public:
  class Options;

  /// \brief construct a new Model
  explicit Model(std::shared_ptr<wgpu::Device> device, const Options&);

  // TODO(ilija@unfolded.ai): Remove once integration is complete and layers provide valid options
  explicit Model(std::shared_ptr<wgpu::Device> device);

  void draw();

  /// \brief Rendering pipeline.
  std::unique_ptr<wgpu::RenderPipeline> pipeline;
  /// \brief Layout of the bind group.
  std::unique_ptr<wgpu::BindGroupLayout> uniformBindGroupLayout;
  /// \brief Compiled vertex shader.
  std::unique_ptr<wgpu::ShaderModule> vsModule;
  /// \brief Compiled fragment shader.
  std::unique_ptr<wgpu::ShaderModule> fsModule;
};

class Model::Options {
 public:
  Options() {}
  Options(const std::string& vs, const std::string& fs, const wgpu::TextureFormat& textureFormat)
      : vs{vs}, fs{fs}, textureFormat{textureFormat} {}

  /// \brief Vertex shader source.
  std::string vs;
  /// \brief Fragment shader source.
  std::string fs;
  /// \brief Texture format that the pipeline will use.
  wgpu::TextureFormat textureFormat;
};

}  // namespace lumagl

#endif  // LUMAGL_CORE_MODEL_H
