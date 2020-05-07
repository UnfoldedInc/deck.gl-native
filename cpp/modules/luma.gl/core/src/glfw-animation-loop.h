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

#ifndef LUMAGL_CORE_GLFW_ANIMATION_LOOP_H
#define LUMAGL_CORE_GLFW_ANIMATION_LOOP_H

#include <memory>
#include <string>

#include "./animation-loop.h"
#include "luma.gl/webgpu/src/backends/backend-binding.h"
#include "luma.gl/webgpu/src/webgpu-utils.h"

struct GLFWwindow;

namespace lumagl {

class GLFWAnimationLoop : public AnimationLoop {
 public:
  using super = AnimationLoop;

  GLFWAnimationLoop(const Size& size = Size{640, 480}, const std::string& windowTitle = "deck.gl",
                    const wgpu::Device& device = nullptr, const wgpu::Queue& queue = nullptr,
                    const wgpu::BackendType backendType = utils::getDefaultWebGPUBackendType());
  ~GLFWAnimationLoop();

  void frame(std::function<void(wgpu::RenderPassEncoder)> onRender) override;

  auto shouldQuit() -> bool override;
  void flush() override;
  auto getPreferredSwapChainTextureFormat() -> wgpu::TextureFormat override;
  auto devicePixelRatio() -> float override;
  void setSize(const Size& size) override;

 private:
  auto _createDevice(const wgpu::BackendType backendType) -> wgpu::Device;
  auto _createSwapchain(wgpu::Device device) -> wgpu::SwapChain;
  auto _initializeGLFW(const wgpu::BackendType backendType, const std::string& windowTitle) -> GLFWwindow*;

  /// \brief Instance used for adapter discovery and device creation. It has to be kept around as Dawn objects'
  /// lifecycle seems to depend on it.
  /// \note From Dawn docs: This is an RAII class for Dawn instances and also controls the lifetime of all adapters
  /// for this instance.
  std::unique_ptr<dawn_native::Instance> _instance{nullptr};
  utils::BackendBinding* _binding{nullptr};
  wgpu::SwapChain _swapchain;

  GLFWwindow* _window{nullptr};
};

}  // namespace lumagl

#endif  // LUMAGL_CORE_GLFW_ANIMATION_LOOP_H
