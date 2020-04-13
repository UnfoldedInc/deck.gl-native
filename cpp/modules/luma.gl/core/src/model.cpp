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

using namespace lumagl;
using namespace lumagl::utils;

Model::Model(std::shared_ptr<wgpu::Device> device) : Model(device, Options{}) {}

Model::Model(std::shared_ptr<wgpu::Device> device, const Model::Options& options) {
  auto deviceValue = *device.get();

  auto vsModule = createShaderModule(deviceValue, SingleShaderStage::Vertex, options.vs.c_str());
  auto fsModule = createShaderModule(deviceValue, SingleShaderStage::Fragment, options.fs.c_str());

  auto uniformBindGroupLayout =
      makeBindGroupLayout(deviceValue, {{0, wgpu::ShaderStage::Vertex, wgpu::BindingType::UniformBuffer, true}});

  ComboRenderPipelineDescriptor descriptor{deviceValue};
  descriptor.vertexStage.module = vsModule;
  descriptor.cFragmentStage.module = fsModule;
  descriptor.cColorStates[0].format = options.textureFormat;
  descriptor.layout = makeBasicPipelineLayout(deviceValue, &uniformBindGroupLayout);

  auto pipeline = deviceValue.CreateRenderPipeline(&descriptor);

  this->vsModule = std::make_unique<wgpu::ShaderModule>(std::move(vsModule));
  this->fsModule = std::make_unique<wgpu::ShaderModule>(std::move(fsModule));
  this->uniformBindGroupLayout = std::make_unique<wgpu::BindGroupLayout>(std::move(uniformBindGroupLayout));
  this->pipeline = std::make_unique<wgpu::RenderPipeline>(std::move(pipeline));
}

void Model::draw() {
  // TODO(ilija@unfolded.ai): Implement
}
