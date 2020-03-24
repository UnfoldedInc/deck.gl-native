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

namespace lumagl {

class AnimationLoop {
 public:
  explicit AnimationLoop(wgpu::Device device = nullptr);

  void setSize(int width, int height);

  void run(std::function<void(wgpu::RenderPassEncoder)>);
  void frame(std::function<void(wgpu::RenderPassEncoder)>);
  void stop();

  // std::function<bool(AnimationLoop *)> onNeedsRedraw{[](AnimationLoop *) { return true; }};
  // std::function<void(AnimationLoop *)> onBeforeRender{[](AnimationLoop *) {}};
  // std::function<void(AnimationLoop *)> onRender{[](AnimationLoop *, ) {}};
  // std::function<void(AnimationLoop *)> onAfterRender{[](AnimationLoop *) {}};

  virtual auto createDevice(wgpu::BackendType) -> wgpu::Device;
  virtual bool shouldQuit() { return false; }
  virtual void flush() {}

  int width{640};
  int height{480};

  // Internal, but left public to facilitate integration
  wgpu::Device device;
  wgpu::Queue queue;
  wgpu::SwapChain swapchain;

  bool running{false};
};

}  // namespace lumagl

#endif  // LUMAGL_CORE_ANIMATION_LOOP_H
