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

#include "./size.h"

namespace lumagl {

class AnimationLoop {
 public:
  struct Options;

  explicit AnimationLoop(const Options& options);
  virtual ~AnimationLoop() {}

  virtual void draw(std::function<void(wgpu::RenderPassEncoder)> onRender) {}
  virtual void draw(wgpu::TextureView textureView, std::function<void(wgpu::RenderPassEncoder)> onRender);
  virtual void run(std::function<void(wgpu::RenderPassEncoder)> onRender);
  virtual void stop();

  virtual auto shouldQuit() -> bool { return false; }
  virtual void flush() {}
  virtual auto getPreferredSwapChainTextureFormat() -> wgpu::TextureFormat { return wgpu::TextureFormat::Undefined; };
  virtual auto devicePixelRatio() -> float { return 1.0f; }
  virtual void setSize(const Size& size);

  auto size() const -> Size { return this->_size; };
  auto device() -> wgpu::Device { return this->_device; }
  auto queue() -> wgpu::Queue { return this->_queue; }

  bool running{false};

 protected:
  void _initialize(wgpu::Device device, wgpu::Queue queue);

  Size _size;
  wgpu::Device _device;

 private:
  wgpu::Queue _queue;
};

struct AnimationLoop::Options {
 public:
  Options(const wgpu::Device& device, const wgpu::Queue& queue = nullptr, const Size& size = Size{640, 480})
      : device{device}, queue{queue}, size{size} {}
  virtual ~Options() = default;

  wgpu::Device device;
  wgpu::Queue queue;
  Size size;
};

}  // namespace lumagl

#endif  // LUMAGL_CORE_ANIMATION_LOOP_H
