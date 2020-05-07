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

#include "./model.h"  // NOLINT(build/include)

#include <algorithm>

#include "luma.gl/garrow/src/util/webgpu-utils.h"
#include "luma.gl/webgpu.h"
#include "math.gl/core.h"
#include "probe.gl/core.h"

using namespace lumagl;
using namespace lumagl::utils;

Model::Model(wgpu::Device device, const Model::Options& options) {
  this->_device = device;
  this->_uniformDescriptors = options.uniforms;

  this->vsModule = createShaderModule(device, SingleShaderStage::Vertex, options.vs.c_str());
  this->fsModule = createShaderModule(device, SingleShaderStage::Fragment, options.fs.c_str());

  ComboRenderPipelineDescriptor descriptor{device};
  descriptor.vertexStage.module = this->vsModule;
  descriptor.cFragmentStage.module = this->fsModule;
  descriptor.primitiveTopology = options.primitiveTopology;

  descriptor.cColorStates[0].format = options.textureFormat;
  descriptor.cColorStates[0].colorBlend.srcFactor = wgpu::BlendFactor::SrcAlpha;
  descriptor.cColorStates[0].colorBlend.dstFactor = wgpu::BlendFactor::OneMinusSrcAlpha;
  descriptor.cColorStates[0].alphaBlend.srcFactor = wgpu::BlendFactor::SrcAlpha;
  descriptor.cColorStates[0].alphaBlend.dstFactor = wgpu::BlendFactor::OneMinusSrcAlpha;

  this->_initializeVertexState(&descriptor.cVertexState, options.attributeSchema, options.instancedAttributeSchema);

  // Initialize uniform cache
  this->_bindings = std::vector<std::optional<BindingInitializationHelper>>{options.uniforms.size()};

  this->uniformBindGroupLayout = this->_createBindGroupLayout(device, options.uniforms);
  descriptor.layout = makeBasicPipelineLayout(device, &this->uniformBindGroupLayout);

  this->pipeline = device.CreateRenderPipeline(&descriptor);

  // TODO(ilija@unfolded.ai): Is there a more elegant way of doing this, other than divering from arrow API and
  // providing a simple way to initialize an empty table?
  auto schema = std::make_shared<garrow::Schema>(std::vector<std::shared_ptr<garrow::Field>>{});
  this->_attributeTable = std::make_shared<garrow::Table>(schema, std::vector<std::shared_ptr<garrow::Array>>{});
  this->_instancedAttributeTable =
      std::make_shared<garrow::Table>(schema, std::vector<std::shared_ptr<garrow::Array>>{});
}

void Model::setAttributes(const std::shared_ptr<garrow::Table>& attributes) {
  // TODO(ilija@unfolded.ai): Compare to schema provided in the constructor?
  this->_attributeTable = attributes;
}

void Model::setInstancedAttributes(const std::shared_ptr<garrow::Table>& attributes) {
  // TODO(ilija@unfolded.ai): Compare to schema provided in the constructor?
  this->_instancedAttributeTable = attributes;
}

void Model::setUniformBuffer(uint32_t binding, const wgpu::Buffer& buffer, uint64_t offset, uint64_t size) {
  this->_setBinding(binding, BindingInitializationHelper{binding, buffer, offset, size});
}

void Model::setUniformTexture(uint32_t binding, const wgpu::TextureView& textureView) {
  this->_setBinding(binding, BindingInitializationHelper{binding, textureView});
}

void Model::setUniformSampler(uint32_t binding, const wgpu::Sampler& sampler) {
  this->_setBinding(binding, BindingInitializationHelper{binding, sampler});
}

void Model::draw(wgpu::RenderPassEncoder pass) {
  pass.SetPipeline(this->pipeline);
  this->_setVertexBuffers(pass);
  // The last two arguments are used for specifying dynamic offsets, which is not something we support right now
  pass.SetBindGroup(0, this->bindGroup, 0, nullptr);

  // Make sure at least one instance is being drawn in case no instanced attributes are present
  uint32_t minimumInstances = 1;
  auto instanceCount = std::max(static_cast<uint32_t>(this->_instancedAttributeTable->num_rows()), minimumInstances);
  pass.Draw(this->vertexCount, instanceCount, 0, 0);
}

void Model::_initializeVertexState(utils::ComboVertexStateDescriptor* descriptor,
                                   const std::shared_ptr<garrow::Schema>& attributeSchema,
                                   const std::shared_ptr<garrow::Schema>& instancedAttributeSchema) {
  descriptor->vertexBufferCount =
      static_cast<uint32_t>(attributeSchema->num_fields() + instancedAttributeSchema->num_fields());

  int location = 0;
  for (auto const& field : attributeSchema->fields()) {
    descriptor->cVertexBuffers[location].arrayStride = lumagl::garrow::getVertexFormatSize(field->type());
    descriptor->cVertexBuffers[location].stepMode = wgpu::InputStepMode::Vertex;
    descriptor->cVertexBuffers[location].attributeCount = 1;
    descriptor->cVertexBuffers[location].attributes = &descriptor->cAttributes[location];

    descriptor->cAttributes[location].shaderLocation = location;
    descriptor->cAttributes[location].format = field->type();

    location++;
  }

  for (auto const& field : instancedAttributeSchema->fields()) {
    descriptor->cVertexBuffers[location].arrayStride = lumagl::garrow::getVertexFormatSize(field->type());
    descriptor->cVertexBuffers[location].stepMode = wgpu::InputStepMode::Instance;
    descriptor->cVertexBuffers[location].attributeCount = 1;
    descriptor->cVertexBuffers[location].attributes = &descriptor->cAttributes[location];

    descriptor->cAttributes[location].shaderLocation = location;
    descriptor->cAttributes[location].format = field->type();

    location++;
  }
}

auto Model::_createBindGroupLayout(wgpu::Device device, const std::vector<UniformDescriptor>& uniforms)
    -> wgpu::BindGroupLayout {
  std::vector<wgpu::BindGroupLayoutBinding> bindings;
  for (uint32_t i = 0; i < uniforms.size(); i++) {
    auto binding =
        wgpu::BindGroupLayoutBinding{i, uniforms[i].shaderStage, uniforms[i].bindingType, uniforms[i].isDynamic};
    bindings.push_back(binding);
  }

  return utils::makeBindGroupLayout(device, bindings);
}

void Model::_setBinding(uint32_t binding, const BindingInitializationHelper& initHelper) {
  this->_bindings[binding] = initHelper;

  // Make sure all uniforms are set before trying to create a bind group
  std::vector<BindingInitializationHelper> bindings;
  for (auto const& binding : this->_bindings) {
    if (binding) {
      bindings.push_back(binding.value());
    } else {
      return;
    }
  }

  // Update the bind group
  this->bindGroup = utils::makeBindGroup(this->_device, this->uniformBindGroupLayout, bindings);
}

void Model::_setVertexBuffers(wgpu::RenderPassEncoder pass) {
  int location = 0;
  for (auto const& attribute : this->_attributeTable->columns()) {
    pass.SetVertexBuffer(location, attribute->buffer());
    location++;
  }

  for (auto const& attribute : this->_instancedAttributeTable->columns()) {
    pass.SetVertexBuffer(location, attribute->buffer());
    location++;
  }
}
