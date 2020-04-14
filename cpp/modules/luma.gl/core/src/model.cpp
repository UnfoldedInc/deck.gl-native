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

using namespace lumagl;
using namespace lumagl::utils;

Model::Model(std::shared_ptr<wgpu::Device> device) : Model(device, Options{}) {}

Model::Model(std::shared_ptr<wgpu::Device> device, const Model::Options& options) {
  this->_device = device;
  auto deviceValue = *device.get();

  auto vsModule = createShaderModule(deviceValue, SingleShaderStage::Vertex, options.vs.c_str());
  auto fsModule = createShaderModule(deviceValue, SingleShaderStage::Fragment, options.fs.c_str());

  // TODO(ilija@unfolded.ai): Build this up based on attribute descriptors. Currently hardcoded to support animometer
  auto uniformBindGroupLayout =
      makeBindGroupLayout(deviceValue, {{0, wgpu::ShaderStage::Vertex, wgpu::BindingType::UniformBuffer, true}});

  ComboRenderPipelineDescriptor descriptor{deviceValue};
  descriptor.vertexStage.module = vsModule;
  descriptor.cFragmentStage.module = fsModule;
  descriptor.cColorStates[0].format = options.textureFormat;

  descriptor.cVertexState.vertexBufferCount = 2;

  descriptor.cVertexState.cVertexBuffers[0].arrayStride = sizeof(mathgl::Vector4<float>);
  descriptor.cVertexState.cVertexBuffers[0].attributeCount = 3;
  descriptor.cVertexState.cVertexBuffers[0].attributes = &descriptor.cVertexState.cAttributes[0];

  descriptor.cVertexState.cVertexBuffers[1].arrayStride = sizeof(mathgl::Vector4<float>);
  descriptor.cVertexState.cVertexBuffers[1].attributeCount = 3;
  descriptor.cVertexState.cVertexBuffers[1].attributes = &descriptor.cVertexState.cAttributes[3];

  for (int i = 0; i < 6; i++) {
    descriptor.cVertexState.cAttributes[i].shaderLocation = i;
    descriptor.cVertexState.cAttributes[i].format = wgpu::VertexFormat::Float4;
  }

  descriptor.layout = makeBasicPipelineLayout(deviceValue, &uniformBindGroupLayout);

  auto pipeline = deviceValue.CreateRenderPipeline(&descriptor);

  this->vsModule = std::make_unique<wgpu::ShaderModule>(std::move(vsModule));
  this->fsModule = std::make_unique<wgpu::ShaderModule>(std::move(fsModule));
  this->uniformBindGroupLayout = std::make_unique<wgpu::BindGroupLayout>(std::move(uniformBindGroupLayout));
  this->pipeline = std::make_unique<wgpu::RenderPipeline>(std::move(pipeline));
}

void Model::setAttributes(const std::shared_ptr<WebGPUTable>& table) { this->_attributes = table; }

void Model::draw(wgpu::RenderPassEncoder pass) {
  if (!this->_attributes) {
    return;
  }

  std::vector<BindingInitializationHelper> bindings;
  for (auto& attribute : this->_attributes->columns) {
    BindingInitializationHelper bindingHelper{0, attribute->buffer, 0, attribute->elementSize};
    bindings.push_back(bindingHelper);
  }

  wgpu::BindGroup bindGroup =
      utils::makeBindGroup(*(this->_device.get()), *(this->uniformBindGroupLayout.get()), bindings);

  pass.SetPipeline(*this->pipeline.get());

  // TODO(ilija@unfolded.ai): Revisit
  for (size_t i = 0; i < this->_attributes->numRows; i++) {
    //    uint32_t offset = static_cast<uint32_t>(i * sizeof(ShaderData));
    //    pass.SetBindGroup(0, bindGroup, 1, &offset);
    //    pass.Draw(3, 1, 0, 0);
  }
}
