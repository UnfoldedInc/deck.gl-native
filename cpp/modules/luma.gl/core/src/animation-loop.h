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

#include <dawn/dawn_proc.h>
#include <dawn/webgpu_cpp.h>

#include <functional>
#include <memory>

namespace lumagl {

struct Size {
 public:
  Size(int width, int height) : width{width}, height{height} {}

  int width;
  int height;
};

class AnimationLoop {
 public:
  explicit AnimationLoop(const Size& size = Size{640, 480});
  virtual ~AnimationLoop();

  auto size() -> Size { return this->_size; };
  void setSize(const Size& size);

  virtual void run(std::function<void(wgpu::RenderPassEncoder)> onRender);
  virtual void frame(std::function<void(wgpu::RenderPassEncoder)> onRender);
  virtual void stop();

  virtual bool shouldQuit() { return false; }
  virtual void flush() {}
  virtual auto getPreferredSwapChainTextureFormat() -> wgpu::TextureFormat { return wgpu::TextureFormat::Undefined; };

  bool running{false};
  auto device() -> wgpu::Device { return this->_device; }

 protected:
  void _initialize(const wgpu::BackendType backendType, wgpu::Device device);
  virtual auto _createDevice(const wgpu::BackendType backendType) -> wgpu::Device = 0;
  virtual auto _createSwapchain(wgpu::Device device) -> wgpu::SwapChain = 0;

  Size _size;

 private:
  DawnProcTable _procs;
  wgpu::Device _device;
  wgpu::Queue _queue;
  wgpu::SwapChain _swapchain;
};

}  // namespace lumagl

#endif  // LUMAGL_CORE_ANIMATION_LOOP_H
