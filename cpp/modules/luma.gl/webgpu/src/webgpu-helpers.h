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

#ifndef LUMAGL_WEBGPU_HELPERS_H
#define LUMAGL_WEBGPU_HELPERS_H

#include <dawn/webgpu_cpp.h>

#include <array>
#include <initializer_list>
#include <memory>
#include <vector>

#include "./webgpu-constants.h"

namespace lumagl {
namespace utils {

enum Expectation { Success, Failure };

auto createBufferFromData(const wgpu::Device& device, const void* data, uint64_t size, wgpu::BufferUsage usage)
    -> wgpu::Buffer;

template <typename T>
auto createBufferFromData(const wgpu::Device& device, wgpu::BufferUsage usage, std::initializer_list<T> data)
    -> wgpu::Buffer {
  return createBufferFromData(device, data.begin(), uint32_t(sizeof(T) * data.size()), usage);
}

auto createBufferCopyView(wgpu::Buffer buffer, uint64_t offset, uint32_t rowPitch, uint32_t imageHeight)
    -> wgpu::BufferCopyView;
auto createTextureCopyView(wgpu::Texture texture, uint32_t level, uint32_t slice, wgpu::Origin3D origin)
    -> wgpu::TextureCopyView;

struct ComboRenderPassDescriptor : public wgpu::RenderPassDescriptor {
 public:
  ComboRenderPassDescriptor(std::initializer_list<wgpu::TextureView> colorAttachmentInfo,
                            wgpu::TextureView depthStencil = wgpu::TextureView());
  const ComboRenderPassDescriptor& operator=(const ComboRenderPassDescriptor& otherRenderPass);

  std::array<wgpu::RenderPassColorAttachmentDescriptor, kMaxColorAttachments> cColorAttachments;
  wgpu::RenderPassDepthStencilAttachmentDescriptor cDepthStencilAttachmentInfo;
};

struct BasicRenderPass {
 public:
  BasicRenderPass();
  BasicRenderPass(uint32_t width, uint32_t height, wgpu::Texture color,
                  wgpu::TextureFormat texture = kDefaultColorFormat);

  static constexpr wgpu::TextureFormat kDefaultColorFormat = wgpu::TextureFormat::RGBA8Unorm;

  uint32_t width;
  uint32_t height;
  wgpu::Texture color;
  wgpu::TextureFormat colorFormat;
  utils::ComboRenderPassDescriptor renderPassInfo;
};

auto createBasicRenderPass(const wgpu::Device& device, uint32_t width, uint32_t height) -> BasicRenderPass;

auto getDefaultSamplerDescriptor() -> wgpu::SamplerDescriptor;
auto makeBasicPipelineLayout(const wgpu::Device& device, const wgpu::BindGroupLayout* bindGroupLayout)
    -> wgpu::PipelineLayout;
auto makeBindGroupLayout(const wgpu::Device& device, std::vector<wgpu::BindGroupLayoutBinding> bindingsInitializer)
    -> wgpu::BindGroupLayout;

auto createDefaultDepthStencilView(const wgpu::Device& device) -> wgpu::TextureView;

// Helpers to make creating bind groups look nicer:
//
//   utils::MakeBindGroup(device, layout, {
//       {0, mySampler},
//       {1, myBuffer, offset, size},
//       {3, myTextureView}
//   });

// Structure with one constructor per-type of bindings, so that the initializer_list accepts
// bindings with the right type and no extra information.
struct BindingInitializationHelper {
  BindingInitializationHelper(uint32_t binding, const wgpu::Sampler& sampler);
  BindingInitializationHelper(uint32_t binding, const wgpu::TextureView& textureView);
  BindingInitializationHelper(uint32_t binding, const wgpu::Buffer& buffer, uint64_t offset = 0,
                              uint64_t size = wgpu::kWholeSize);

  wgpu::BindGroupBinding GetAsBinding() const;

  uint32_t binding;
  wgpu::Sampler sampler;
  wgpu::TextureView textureView;
  wgpu::Buffer buffer;
  uint64_t offset = 0;
  uint64_t size = 0;
};

auto makeBindGroup(const wgpu::Device& device, const wgpu::BindGroupLayout& layout,
                   std::vector<BindingInitializationHelper> bindingsInitializer) -> wgpu::BindGroup;
auto makeBindGroup(const wgpu::Device& device, const wgpu::BindGroupLayout& layout,
                   std::vector<std::shared_ptr<BindingInitializationHelper>> bindingsInitializer) -> wgpu::BindGroup;

}  // namespace utils
}  // namespace lumagl

#endif  // LUMAGL_WEBGPU_HELPERS_H
