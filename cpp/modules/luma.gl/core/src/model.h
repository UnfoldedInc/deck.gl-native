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
#include <vector>

#include "luma.gl/garrow.h"
#include "luma.gl/webgpu.h"

namespace lumagl {

// TODO(ilija@unfolded.ai): Move out and revisit
struct UniformDescriptor {
  size_t elementSize;
  bool isDynamic{false};
};

/// \brief Holds shaders compiled and linked into a pipeline
class Model {
 public:
  class Options;

  /// \brief construct a new Model
  explicit Model(std::shared_ptr<wgpu::Device> device, const Options&);

  // TODO(ilija@unfolded.ai): Remove once integration is complete and layers provide valid options
  explicit Model(std::shared_ptr<wgpu::Device> device);

  void setAttributes(const std::shared_ptr<garrow::Table>& attributes);
  void setUniformBuffers(const std::vector<wgpu::Buffer>& uniforms);

  void draw(wgpu::RenderPassEncoder pass);

  int vertexCount;

  /// \brief Rendering pipeline.
  wgpu::RenderPipeline pipeline;
  /// \brief Layout of the bind group.
  wgpu::BindGroupLayout uniformBindGroupLayout;
  /// \brief Bind group containg uniform data.
  wgpu::BindGroup bindGroup;
  /// \brief Compiled vertex shader.
  wgpu::ShaderModule vsModule;
  /// \brief Compiled fragment shader.
  wgpu::ShaderModule fsModule;

 private:
  std::shared_ptr<wgpu::Device> _device;
  std::shared_ptr<garrow::Table> _attributeTable;
  std::vector<UniformDescriptor> _uniforms;

  void _initializeVertexState(utils::ComboVertexStateDescriptor&, const std::shared_ptr<garrow::Schema>&);
  auto _createBindGroupLayout(wgpu::Device device, const std::vector<UniformDescriptor>& uniforms)
      -> wgpu::BindGroupLayout;

  void _setVertexBuffers(wgpu::RenderPassEncoder pass);
};

class Model::Options {
 public:
  Options(const std::string& vs, const std::string& fs, const std::shared_ptr<garrow::Schema>& attributeSchema,
          const std::vector<UniformDescriptor>& uniforms = {},
          const wgpu::TextureFormat& textureFormat = static_cast<wgpu::TextureFormat>(WGPUTextureFormat_BGRA8Unorm))
      : vs{vs}, fs{fs}, attributeSchema{attributeSchema}, uniforms{uniforms}, textureFormat{textureFormat} {}

  /// \brief Vertex shader source.
  std::string vs;
  /// \brief Fragment shader source.
  std::string fs;
  /// \brief Attribute definitions.
  const std::shared_ptr<garrow::Schema> attributeSchema;
  /// \brief Uniform definitions.
  const std::vector<UniformDescriptor> uniforms;
  /// \brief Texture format that the pipeline will use.
  wgpu::TextureFormat textureFormat;
};

}  // namespace lumagl

#endif  // LUMAGL_CORE_MODEL_H
