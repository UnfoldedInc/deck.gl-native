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

/// \brief Collection of keys that provide additional attribute context when present in input scheme metadata.
// TODO(ilija@unfolded.ai): Document metadata properties and the behavior they trigger once we finalize the list
struct AttributePropertyKeys {};

// TODO(ilija@unfolded.ai): Move out and revisit
struct UniformDescriptor {
  size_t elementSize;
  bool isDynamic{false};
};

/// \brief Holds shaders compiled and linked into a pipeline.
class Model {
 public:
  class Options;

  /// \brief construct a new Model.
  Model(wgpu::Device device, const Options&);

  void setAttributes(const std::shared_ptr<garrow::Table>& attributes);
  void setInstancedAttributes(const std::shared_ptr<garrow::Table>& attributes);

  void setUniforms(const std::vector<std::shared_ptr<garrow::Array>>& uniforms);
  void setUniforms(const std::shared_ptr<garrow::Array>& uniforms, uint32_t index);

  void draw(wgpu::RenderPassEncoder pass);

  auto device() -> wgpu::Device { return this->_device; }

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
  void _initializeVertexState(utils::ComboVertexStateDescriptor* descriptor,
                              const std::shared_ptr<garrow::Schema>& attributeSchema,
                              const std::shared_ptr<garrow::Schema>& instancedAttributeSchema);
  auto _createBindGroupLayout(wgpu::Device device, const std::vector<UniformDescriptor>& uniforms)
      -> wgpu::BindGroupLayout;

  void _setVertexBuffers(wgpu::RenderPassEncoder pass);

  wgpu::Device _device;
  std::shared_ptr<garrow::Table> _attributeTable;
  std::shared_ptr<garrow::Table> _instancedAttributeTable;
  std::vector<UniformDescriptor> _uniformDescriptors;
  // Retaining a reference to uniform buffers so they don't go out of scope before drawing
  std::vector<std::shared_ptr<garrow::Array>> _uniforms;
  std::vector<std::shared_ptr<utils::BindingInitializationHelper>> _bindings;
};

/// \brief Initializer options for the Model class.
class Model::Options {
 public:
  Options(const std::string& vs, const std::string& fs, const std::shared_ptr<garrow::Schema>& attributeSchema,
          const std::shared_ptr<garrow::Schema>& instancedAttributeSchema =
              std::make_shared<garrow::Schema>(std::vector<std::shared_ptr<garrow::Field>>{}),
          const std::vector<UniformDescriptor>& uniforms = {},
          const wgpu::PrimitiveTopology topology = wgpu::PrimitiveTopology::TriangleList,
          const wgpu::TextureFormat& textureFormat = static_cast<wgpu::TextureFormat>(WGPUTextureFormat_BGRA8Unorm))
      : vs{vs},
        fs{fs},
        attributeSchema{attributeSchema},
        instancedAttributeSchema{instancedAttributeSchema},
        uniforms{uniforms},
        topology{topology},
        textureFormat{textureFormat} {}

  /// \brief Vertex shader source.
  std::string vs;
  /// \brief Fragment shader source.
  std::string fs;
  /// \brief Attribute definitions.
  const std::shared_ptr<garrow::Schema> attributeSchema;
  /// \brief Instanced attribute definitions.
  const std::shared_ptr<garrow::Schema> instancedAttributeSchema;
  /// \brief Uniform definitions.
  const std::vector<UniformDescriptor> uniforms;
  /// \brief Type of geometry topology that will be contained in vertex buffers.
  const wgpu::PrimitiveTopology topology;
  /// \brief Texture format that the pipeline will use.
  wgpu::TextureFormat textureFormat;
};

}  // namespace lumagl

#endif  // LUMAGL_CORE_MODEL_H
