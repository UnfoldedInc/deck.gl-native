// Copyright 2017 The Dawn Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef LUMAGL_UTILS_WEBGPU_HELPERS_H
#define LUMAGL_UTILS_WEBGPU_HELPERS_H

#include <dawn/webgpu_cpp.h>

#include <array>
#include <initializer_list>

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
auto makeBindGroupLayout(const wgpu::Device& device,
                         std::initializer_list<wgpu::BindGroupLayoutBinding> bindingsInitializer)
    -> wgpu::BindGroupLayout;

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
                   std::initializer_list<BindingInitializationHelper> bindingsInitializer) -> wgpu::BindGroup;

}  // namespace utils
}  // namespace lumagl

#endif  // LUMAGL_UTILS_WEBGPU_HELPERS_H
