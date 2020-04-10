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

#ifndef LUMAGL_CORE_ANIMATION_LOOP_H
#define LUMAGL_CORE_ANIMATION_LOOP_H

#include <dawn/webgpu_cpp.h>

#include <functional>
#include <memory>

namespace lumagl {

class AnimationLoop {
 public:
  virtual ~AnimationLoop();

  void setSize(int width, int height);

  virtual void run(std::function<void(wgpu::RenderPassEncoder)> onRender);
  virtual void frame(std::function<void(wgpu::RenderPassEncoder)> onRender);
  virtual void stop();

  virtual bool shouldQuit() { return false; }
  virtual void flush() {}
  virtual auto getPreferredSwapChainTextureFormat() -> wgpu::TextureFormat { return wgpu::TextureFormat::Undefined; };

  // TODO(ilija@unfolded.ai): Make these read-only?
  int width{640};
  int height{480};

  // Internal, but left public to facilitate integration
  std::shared_ptr<wgpu::Device> device;
  std::unique_ptr<wgpu::Queue> queue;
  std::unique_ptr<wgpu::SwapChain> swapchain;

  // TODO(ilija@unfolded.ai): Make this read-only?
  bool running{false};

 protected:
  void _initialize(const wgpu::BackendType backendType, std::shared_ptr<wgpu::Device> device);
  virtual auto _createDevice(const wgpu::BackendType backendType) -> std::unique_ptr<wgpu::Device> = 0;
  virtual auto _createSwapchain(std::shared_ptr<wgpu::Device> device) -> std::unique_ptr<wgpu::SwapChain> = 0;
};

}  // namespace lumagl

#endif  // LUMAGL_CORE_ANIMATION_LOOP_H
