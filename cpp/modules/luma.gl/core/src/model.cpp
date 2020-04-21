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

#include "luma.gl/garrow/src/util/webgpu-utils.h"
#include "luma.gl/webgpu.h"
#include "math.gl/core.h"
#include "probe.gl/core.h"

using namespace lumagl;
using namespace lumagl::utils;

Model::Model(std::shared_ptr<wgpu::Device> device, const Model::Options& options) {
  this->_device = device;
  auto deviceValue = *device.get();
  this->_uniforms = options.uniforms;

  this->vsModule = createShaderModule(deviceValue, SingleShaderStage::Vertex, options.vs.c_str());
  this->fsModule = createShaderModule(deviceValue, SingleShaderStage::Fragment, options.fs.c_str());

  ComboRenderPipelineDescriptor descriptor{deviceValue};
  descriptor.vertexStage.module = this->vsModule;
  descriptor.cFragmentStage.module = this->fsModule;
  descriptor.cColorStates[0].format = options.textureFormat;

  this->_initializeVertexState(descriptor.cVertexState, options.attributeSchema);

  this->uniformBindGroupLayout = this->_createBindGroupLayout(deviceValue, options.uniforms);
  descriptor.layout = makeBasicPipelineLayout(deviceValue, &this->uniformBindGroupLayout);

  this->pipeline = deviceValue.CreateRenderPipeline(&descriptor);
}

void Model::setAttributes(const std::shared_ptr<garrow::Table>& attributes) { this->_attributeTable = attributes; }

void Model::setUniformBuffers(const std::vector<wgpu::Buffer>& uniformBuffers) {
  std::vector<BindingInitializationHelper> bindings;
  for (uint32_t i = 0; i < uniformBuffers.size(); i++) {
    auto binding = BindingInitializationHelper{i, uniformBuffers[i], 0, this->_uniforms[i].elementSize};
    bindings.push_back(binding);
  }

  // Update the bind group
  this->bindGroup = utils::makeBindGroup(*this->_device.get(), this->uniformBindGroupLayout, bindings);
}

void Model::draw(wgpu::RenderPassEncoder pass) {
  pass.SetPipeline(this->pipeline);
  this->_setVertexBuffers(pass);
  // The last two arguments are used for specifying dynamic offsets, which is not something we support right now
  pass.SetBindGroup(0, this->bindGroup, 0, nullptr);
  pass.Draw(this->vertexCount, 1, 0, 0);
}

void Model::_initializeVertexState(ComboVertexStateDescriptor& cVertexState,
                                   const std::shared_ptr<garrow::Schema>& attributeSchema) {
  cVertexState.vertexBufferCount = attributeSchema->num_fields();

  for (int location = 0; location < attributeSchema->num_fields(); location++) {
    auto format = attributeSchema->field(location)->type();

    cVertexState.cVertexBuffers[location].arrayStride = lumagl::garrow::getVertexFormatSize(format);
    cVertexState.cVertexBuffers[location].attributeCount = 1;
    cVertexState.cVertexBuffers[location].attributes = &cVertexState.cAttributes[location];

    cVertexState.cAttributes[location].shaderLocation = location;
    cVertexState.cAttributes[location].format = format;
  }
}

auto Model::_createBindGroupLayout(wgpu::Device device, const std::vector<UniformDescriptor>& uniforms)
    -> wgpu::BindGroupLayout {
  std::vector<wgpu::BindGroupLayoutBinding> bindings;
  for (uint32 i = 0; i < uniforms.size(); i++) {
    auto binding = wgpu::BindGroupLayoutBinding{i, wgpu::ShaderStage::Vertex, wgpu::BindingType::UniformBuffer,
                                                uniforms[i].isDynamic};
    bindings.push_back(binding);
  }

  return utils::makeBindGroupLayout(device, bindings);
}

void Model::_setVertexBuffers(wgpu::RenderPassEncoder pass) {
  for (int location = 0; location < this->_attributeTable->num_columns(); location++) {
    pass.SetVertexBuffer(location, this->_attributeTable->column(location)->buffer());
  }
}
