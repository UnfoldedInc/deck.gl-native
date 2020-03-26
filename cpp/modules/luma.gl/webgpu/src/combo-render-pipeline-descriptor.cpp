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

// Note: This file was inspired by the Dawn codebase at https://dawn.googlesource.com/dawn/
// Copyright 2017 The Dawn Authors http://www.apache.org/licenses/LICENSE-2.0

#include "./combo-render-pipeline-descriptor.h"

#include "./webgpu-helpers.h"

namespace lumagl {
namespace utils {

ComboVertexStateDescriptor::ComboVertexStateDescriptor() {
  wgpu::VertexStateDescriptor* descriptor = this;

  descriptor->indexFormat = wgpu::IndexFormat::Uint32;
  descriptor->vertexBufferCount = 0;

  // Fill the default values for vertexBuffers and vertexAttributes in buffers.
  wgpu::VertexAttributeDescriptor vertexAttribute;
  vertexAttribute.shaderLocation = 0;
  vertexAttribute.offset = 0;
  vertexAttribute.format = wgpu::VertexFormat::Float;
  for (uint32_t i = 0; i < kMaxVertexAttributes; ++i) {
    cAttributes[i] = vertexAttribute;
  }
  for (uint32_t i = 0; i < kMaxVertexBuffers; ++i) {
    cVertexBuffers[i].arrayStride = 0;
    cVertexBuffers[i].stepMode = wgpu::InputStepMode::Vertex;
    cVertexBuffers[i].attributeCount = 0;
    cVertexBuffers[i].attributes = nullptr;
  }
  // cVertexBuffers[i].attributes points to somewhere in cAttributes.
  // cVertexBuffers[0].attributes points to &cAttributes[0] by default. Assuming
  // cVertexBuffers[0] has two attributes, then cVertexBuffers[1].attributes should point to
  // &cAttributes[2]. Likewise, if cVertexBuffers[1] has 3 attributes, then
  // cVertexBuffers[2].attributes should point to &cAttributes[5].
  cVertexBuffers[0].attributes = &cAttributes[0];
  descriptor->vertexBuffers = &cVertexBuffers[0];
}

ComboRenderPipelineDescriptor::ComboRenderPipelineDescriptor(const wgpu::Device& device) {
  wgpu::RenderPipelineDescriptor* descriptor = this;

  descriptor->primitiveTopology = wgpu::PrimitiveTopology::TriangleList;
  descriptor->sampleCount = 1;

  // Set defaults for the vertex stage descriptor.
  { vertexStage.entryPoint = "main"; }

  // Set defaults for the fragment stage desriptor.
  {
    descriptor->fragmentStage = &cFragmentStage;
    cFragmentStage.entryPoint = "main";
  }

  // Set defaults for the input state descriptors.
  descriptor->vertexState = &cVertexState;

  // Set defaults for the rasterization state descriptor.
  {
    cRasterizationState.frontFace = wgpu::FrontFace::CCW;
    cRasterizationState.cullMode = wgpu::CullMode::None;

    cRasterizationState.depthBias = 0;
    cRasterizationState.depthBiasSlopeScale = 0.0;
    cRasterizationState.depthBiasClamp = 0.0;
    descriptor->rasterizationState = &cRasterizationState;
  }

  // Set defaults for the color state descriptors.
  {
    descriptor->colorStateCount = 1;
    descriptor->colorStates = cColorStates.data();

    wgpu::BlendDescriptor blend;
    blend.operation = wgpu::BlendOperation::Add;
    blend.srcFactor = wgpu::BlendFactor::One;
    blend.dstFactor = wgpu::BlendFactor::Zero;
    wgpu::ColorStateDescriptor colorStateDescriptor;
    colorStateDescriptor.format = wgpu::TextureFormat::RGBA8Unorm;
    colorStateDescriptor.alphaBlend = blend;
    colorStateDescriptor.colorBlend = blend;
    colorStateDescriptor.writeMask = wgpu::ColorWriteMask::All;
    for (uint32_t i = 0; i < kMaxColorAttachments; ++i) {
      cColorStates[i] = colorStateDescriptor;
    }
  }

  // Set defaults for the depth stencil state descriptors.
  {
    wgpu::StencilStateFaceDescriptor stencilFace;
    stencilFace.compare = wgpu::CompareFunction::Always;
    stencilFace.failOp = wgpu::StencilOperation::Keep;
    stencilFace.depthFailOp = wgpu::StencilOperation::Keep;
    stencilFace.passOp = wgpu::StencilOperation::Keep;

    cDepthStencilState.format = wgpu::TextureFormat::Depth24PlusStencil8;
    cDepthStencilState.depthWriteEnabled = false;
    cDepthStencilState.depthCompare = wgpu::CompareFunction::Always;
    cDepthStencilState.stencilBack = stencilFace;
    cDepthStencilState.stencilFront = stencilFace;
    cDepthStencilState.stencilReadMask = 0xff;
    cDepthStencilState.stencilWriteMask = 0xff;
    descriptor->depthStencilState = nullptr;
  }
}

}  // namespace utils
}  // namespace lumagl
