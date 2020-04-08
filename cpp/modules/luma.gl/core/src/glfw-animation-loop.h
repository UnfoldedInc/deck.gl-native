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

#include <GLFW/glfw3.h>
#include <dawn_wire/WireClient.h>
#include <dawn_wire/WireServer.h>

#include "./animation-loop.h"
#include "luma.gl/webgpu/src/backends/backend-binding.h"
#include "luma.gl/webgpu/src/terrible-command-buffer.h"

namespace lumagl {

class GLFWAnimationLoop : public AnimationLoop {
 public:
  explicit GLFWAnimationLoop(wgpu::BackendType backendType);

  auto createDevice(wgpu::BackendType) -> wgpu::Device override;
  bool shouldQuit() override;
  void flush() override;

  auto getPreferredSwapChainTextureFormat() -> wgpu::TextureFormat;
  auto getSwapChain(const wgpu::Device& device) -> wgpu::SwapChain;

  GLFWwindow* window{nullptr};

 private:
  void _initializeGLFW(wgpu::BackendType);
  uint64_t _getSwapChainImplementation();
  auto _createCppDawnDevice(wgpu::BackendType backendType) -> wgpu::Device;
  auto _createDefaultDepthStencilView(const wgpu::Device& device) -> wgpu::TextureView;

  std::unique_ptr<dawn_native::Instance> _instance;
  utils::BackendBinding* _binding{nullptr};

  dawn_wire::WireServer* _wireServer{nullptr};
  dawn_wire::WireClient* _wireClient{nullptr};
  utils::TerribleCommandBuffer* _c2sBuf{nullptr};
  utils::TerribleCommandBuffer* _s2cBuf{nullptr};
};

}  // namespace lumagl

#endif  // LUMAGL_CORE_GLFW_ANIMATION_LOOP_H
