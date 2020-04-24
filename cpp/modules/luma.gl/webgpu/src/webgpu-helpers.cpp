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

#include "./webgpu-helpers.h"  // NOLINT(build/include)

#include <cstring>
#include <iomanip>
#include <sstream>
#include <vector>

#include "probe.gl/core.h"

using namespace lumagl;
using namespace lumagl::utils;

namespace lumagl {
namespace utils {

auto createBufferFromData(const wgpu::Device& device, const void* data, uint64_t size, wgpu::BufferUsage usage)
    -> wgpu::Buffer {
  wgpu::BufferDescriptor descriptor;
  descriptor.size = size;
  descriptor.usage = usage | wgpu::BufferUsage::CopyDst;

  wgpu::Buffer buffer = device.CreateBuffer(&descriptor);
  buffer.SetSubData(0, size, data);
  return buffer;
}

auto createBufferCopyView(wgpu::Buffer buffer, uint64_t offset, uint32_t rowPitch, uint32_t imageHeight)
    -> wgpu::BufferCopyView {
  wgpu::BufferCopyView bufferCopyView;
  bufferCopyView.buffer = buffer;
  bufferCopyView.offset = offset;
  bufferCopyView.rowPitch = rowPitch;
  bufferCopyView.imageHeight = imageHeight;

  return bufferCopyView;
}

auto createTextureCopyView(wgpu::Texture texture, uint32_t mipLevel, uint32_t arrayLayer, wgpu::Origin3D origin)
    -> wgpu::TextureCopyView {
  wgpu::TextureCopyView textureCopyView;
  textureCopyView.texture = texture;
  textureCopyView.mipLevel = mipLevel;
  textureCopyView.arrayLayer = arrayLayer;
  textureCopyView.origin = origin;

  return textureCopyView;
}

ComboRenderPassDescriptor::ComboRenderPassDescriptor(std::initializer_list<wgpu::TextureView> colorAttachmentInfo,
                                                     wgpu::TextureView depthStencil) {
  for (uint32_t i = 0; i < kMaxColorAttachments; ++i) {
    cColorAttachments[i].loadOp = wgpu::LoadOp::Clear;
    cColorAttachments[i].storeOp = wgpu::StoreOp::Store;
    cColorAttachments[i].clearColor = {0.0f, 0.0f, 0.0f, 0.0f};
  }

  cDepthStencilAttachmentInfo.clearDepth = 1.0f;
  cDepthStencilAttachmentInfo.clearStencil = 0;
  cDepthStencilAttachmentInfo.depthLoadOp = wgpu::LoadOp::Clear;
  cDepthStencilAttachmentInfo.depthStoreOp = wgpu::StoreOp::Store;
  cDepthStencilAttachmentInfo.stencilLoadOp = wgpu::LoadOp::Clear;
  cDepthStencilAttachmentInfo.stencilStoreOp = wgpu::StoreOp::Store;

  colorAttachmentCount = static_cast<uint32_t>(colorAttachmentInfo.size());
  uint32_t colorAttachmentIndex = 0;
  for (const wgpu::TextureView& colorAttachment : colorAttachmentInfo) {
    if (colorAttachment.Get() != nullptr) {
      cColorAttachments[colorAttachmentIndex].attachment = colorAttachment;
    }
    ++colorAttachmentIndex;
  }
  colorAttachments = cColorAttachments.data();

  if (depthStencil.Get() != nullptr) {
    cDepthStencilAttachmentInfo.attachment = depthStencil;
    depthStencilAttachment = &cDepthStencilAttachmentInfo;
  } else {
    depthStencilAttachment = nullptr;
  }
}

const ComboRenderPassDescriptor& ComboRenderPassDescriptor::operator=(
    const ComboRenderPassDescriptor& otherRenderPass) {
  cDepthStencilAttachmentInfo = otherRenderPass.cDepthStencilAttachmentInfo;
  cColorAttachments = otherRenderPass.cColorAttachments;
  colorAttachmentCount = otherRenderPass.colorAttachmentCount;

  colorAttachments = cColorAttachments.data();

  if (otherRenderPass.depthStencilAttachment != nullptr) {
    // Assign desc.depthStencilAttachment to this->depthStencilAttachmentInfo;
    depthStencilAttachment = &cDepthStencilAttachmentInfo;
  } else {
    depthStencilAttachment = nullptr;
  }

  return *this;
}

BasicRenderPass::BasicRenderPass()
    : width{0}, height{0}, color{nullptr}, colorFormat{wgpu::TextureFormat::RGBA8Unorm}, renderPassInfo{{}} {}

BasicRenderPass::BasicRenderPass(uint32_t texWidth, uint32_t texHeight, wgpu::Texture colorAttachment,
                                 wgpu::TextureFormat textureFormat)
    : width{texWidth},
      height{texHeight},
      color{colorAttachment},
      colorFormat{textureFormat},
      renderPassInfo{{colorAttachment.CreateView()}} {}

auto createBasicRenderPass(const wgpu::Device& device, uint32_t width, uint32_t height) -> BasicRenderPass {
  ASSERT(width > 0 && height > 0);

  wgpu::TextureDescriptor descriptor;
  descriptor.dimension = wgpu::TextureDimension::e2D;
  descriptor.size.width = width;
  descriptor.size.height = height;
  descriptor.size.depth = 1;
  descriptor.arrayLayerCount = 1;
  descriptor.sampleCount = 1;
  descriptor.format = BasicRenderPass::kDefaultColorFormat;
  descriptor.mipLevelCount = 1;
  descriptor.usage = wgpu::TextureUsage::OutputAttachment | wgpu::TextureUsage::CopySrc;
  wgpu::Texture color = device.CreateTexture(&descriptor);

  return BasicRenderPass(width, height, color);
}

auto getDefaultSamplerDescriptor() -> wgpu::SamplerDescriptor {
  wgpu::SamplerDescriptor desc;

  desc.minFilter = wgpu::FilterMode::Linear;
  desc.magFilter = wgpu::FilterMode::Linear;
  desc.mipmapFilter = wgpu::FilterMode::Linear;
  desc.addressModeU = wgpu::AddressMode::Repeat;
  desc.addressModeV = wgpu::AddressMode::Repeat;
  desc.addressModeW = wgpu::AddressMode::Repeat;
  desc.lodMinClamp = kLodMin;
  desc.lodMaxClamp = kLodMax;
  desc.compare = wgpu::CompareFunction::Never;

  return desc;
}

auto makeBasicPipelineLayout(const wgpu::Device& device, const wgpu::BindGroupLayout* bindGroupLayout)
    -> wgpu::PipelineLayout {
  wgpu::PipelineLayoutDescriptor descriptor;
  if (bindGroupLayout != nullptr) {
    descriptor.bindGroupLayoutCount = 1;
    descriptor.bindGroupLayouts = bindGroupLayout;
  } else {
    descriptor.bindGroupLayoutCount = 0;
    descriptor.bindGroupLayouts = nullptr;
  }
  return device.CreatePipelineLayout(&descriptor);
}

auto makeBindGroupLayout(const wgpu::Device& device, std::vector<wgpu::BindGroupLayoutBinding> bindingsInitializer)
    -> wgpu::BindGroupLayout {
  constexpr wgpu::ShaderStage kNoStages{};

  std::vector<wgpu::BindGroupLayoutBinding> bindings;
  for (const wgpu::BindGroupLayoutBinding& binding : bindingsInitializer) {
    if (binding.visibility != kNoStages) {
      bindings.push_back(binding);
    }
  }

  wgpu::BindGroupLayoutDescriptor descriptor;
  descriptor.bindingCount = static_cast<uint32_t>(bindings.size());
  descriptor.bindings = bindings.data();
  return device.CreateBindGroupLayout(&descriptor);
}

auto createDefaultDepthStencilView(const wgpu::Device& device) -> wgpu::TextureView {
  wgpu::TextureDescriptor descriptor;
  descriptor.dimension = wgpu::TextureDimension::e2D;
  descriptor.size.width = 640;
  descriptor.size.height = 480;
  descriptor.size.depth = 1;
  descriptor.arrayLayerCount = 1;
  descriptor.sampleCount = 1;
  descriptor.format = wgpu::TextureFormat::Depth24PlusStencil8;
  descriptor.mipLevelCount = 1;
  descriptor.usage = wgpu::TextureUsage::OutputAttachment;
  auto depthStencilTexture = device.CreateTexture(&descriptor);
  return depthStencilTexture.CreateView();
}

BindingInitializationHelper::BindingInitializationHelper(uint32_t binding, const wgpu::Sampler& sampler)
    : binding{binding}, sampler{sampler} {}

BindingInitializationHelper::BindingInitializationHelper(uint32_t binding, const wgpu::TextureView& textureView)
    : binding{binding}, textureView{textureView} {}

BindingInitializationHelper::BindingInitializationHelper(uint32_t binding, const wgpu::Buffer& buffer, uint64_t offset,
                                                         uint64_t size)
    : binding{binding}, buffer{buffer}, offset{offset}, size{size} {}

wgpu::BindGroupBinding BindingInitializationHelper::GetAsBinding() const {
  wgpu::BindGroupBinding result;

  result.binding = binding;
  result.sampler = sampler;
  result.textureView = textureView;
  result.buffer = buffer;
  result.offset = offset;
  result.size = size;

  return result;
}

auto makeBindGroup(const wgpu::Device& device, const wgpu::BindGroupLayout& layout,
                   std::vector<BindingInitializationHelper> bindingsInitializer) -> wgpu::BindGroup {
  std::vector<std::shared_ptr<BindingInitializationHelper>> pointers;
  std::transform(
      bindingsInitializer.begin(), bindingsInitializer.end(), std::back_inserter(pointers),
      [](BindingInitializationHelper binding) { return std::make_shared<BindingInitializationHelper>(binding); });

  return makeBindGroup(device, layout, pointers);
}

auto makeBindGroup(const wgpu::Device& device, const wgpu::BindGroupLayout& layout,
                   std::vector<std::shared_ptr<BindingInitializationHelper>> bindingsInitializer) -> wgpu::BindGroup {
  std::vector<wgpu::BindGroupBinding> bindings;
  for (auto const& helper : bindingsInitializer) {
    bindings.push_back(helper->GetAsBinding());
  }

  wgpu::BindGroupDescriptor descriptor;
  descriptor.layout = layout;
  descriptor.bindingCount = static_cast<uint32_t>(bindings.size());
  descriptor.bindings = bindings.data();

  return device.CreateBindGroup(&descriptor);
}

}  // namespace utils
}  // namespace lumagl
