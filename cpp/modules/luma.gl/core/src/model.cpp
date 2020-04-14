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

#include "luma.gl/webgpu.h"
#include "math.gl/core.h"
#include "probe.gl/core.h"

using namespace lumagl;
using namespace lumagl::utils;

Model::Model(std::shared_ptr<wgpu::Device> device, const Model::Options& options) {
  this->_device = device;
  auto deviceValue = *device.get();

  this->vsModule = createShaderModule(deviceValue, SingleShaderStage::Vertex, options.vs.c_str());
  this->fsModule = createShaderModule(deviceValue, SingleShaderStage::Fragment, options.fs.c_str());

  // TODO(ilija@unfolded.ai): Build this up based on attribute descriptors. Currently hardcoded to support animometer
  this->uniformBindGroupLayout =
      makeBindGroupLayout(deviceValue, {{0, wgpu::ShaderStage::Vertex, wgpu::BindingType::UniformBuffer, true}});

  ComboRenderPipelineDescriptor descriptor{deviceValue};
  descriptor.vertexStage.module = this->vsModule;
  descriptor.cFragmentStage.module = this->fsModule;
  descriptor.cColorStates[0].format = options.textureFormat;

  this->_initializeVertexState(descriptor.cVertexState, options.attributes);

  descriptor.layout = makeBasicPipelineLayout(deviceValue, &this->uniformBindGroupLayout);

  this->pipeline = deviceValue.CreateRenderPipeline(&descriptor);
}

void Model::setAttributes(const std::shared_ptr<WebGPUTable>& table) { this->_attributes = table; }

void Model::setAttributeBuffers(const std::vector<wgpu::Buffer>& buffers) {
  probegl::DebugLog() << "setting vertex buffers " << buffers.size();
  this->_buffers = buffers;
}

void Model::draw(wgpu::RenderPassEncoder pass) {
  // if (!this->_attributes) {
  //   return;
  // }

  // pass.SetPipeline(this->pipeline);

  this->_setVertexBuffers(pass);

  pass.Draw(3, 1, 0, 0);

  // std::vector<BindingInitializationHelper> bindings;
  // for (auto& attribute : this->_attributes->columns) {
  //   BindingInitializationHelper bindingHelper{0, attribute->buffer, 0, attribute->elementSize};
  //   bindings.push_back(bindingHelper);
  // }

  // wgpu::BindGroup bindGroup = utils::makeBindGroup(*(this->_device.get()), this->uniformBindGroupLayout, bindings);
  // TODO(ilija@unfolded.ai): Revisit
  // for (size_t i = 0; i < this->_attributes->numRows; i++) {
  //    uint32_t offset = static_cast<uint32_t>(i * sizeof(ShaderData));
  //    pass.SetBindGroup(0, bindGroup, 1, &offset);
  //    pass.Draw(3, 1, 0, 0);
  // }
}

void Model::_initializeVertexState(ComboVertexStateDescriptor& cVertexState,
                                   const std::vector<AttributeDescriptor>& attributes) {
  cVertexState.vertexBufferCount = attributes.size();

  probegl::DebugLog() << "initializing vertex attributes " << attributes.size();

  auto location = 0;
  for (auto const attribute : attributes) {
    cVertexState.cVertexBuffers[location].arrayStride = sizeof(mathgl::Vector4<float>);
    cVertexState.cVertexBuffers[location].attributeCount = 1;
    cVertexState.cVertexBuffers[location].attributes = &cVertexState.cAttributes[location];
    location++;
  }

  for (int i = 0; i < attributes.size(); i++) {
    cVertexState.cAttributes[i].shaderLocation = i;
    cVertexState.cAttributes[i].format = wgpu::VertexFormat::Float4;
  }
}

void Model::_setVertexBuffers(wgpu::RenderPassEncoder pass) {
  auto location = 0;
  for (auto const buffer : this->_buffers) {
    probegl::DebugLog() << "installing vertex buffer " << location;
    pass.SetVertexBuffer(location, buffer);
    location++;
  }
}
