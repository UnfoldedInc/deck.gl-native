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

#include "./animation-loop.h"  // NOLINT(build/include)

#include <functional>

#include "luma.gl/webgpu.h"
#include "probe.gl/core.h"

using namespace lumagl;
using namespace lumagl::utils;

AnimationLoop::AnimationLoop(wgpu::Device device_) : device{device_} {
  // Create a device if none was provided
  // this->swapchain =
  this->setSize(640, 480);
}

auto AnimationLoop::createDevice(wgpu::BackendType) -> wgpu::Device { return wgpu::Device(); }

void AnimationLoop::setSize(int width, int height) {
  bool sizeChanged = width != this->width || height != this->height;
  this->width = width;
  this->height = height;

  if (sizeChanged) {
    // auto swapchain = GetSwapChain(device);  TODO(ib): better to just query for swap chain before use?
    // this->swapchain.Configure(GetPreferredSwapChainTextureFormat(), wgpu::TextureUsage::OutputAttachment,
    // this->width,
    //                           this->height);
  }
}

void AnimationLoop::run(std::function<void(wgpu::RenderPassEncoder)> onRender) {
  if (!this->device) {
    this->device = this->createDevice(getDefaultWebGPUBackendType());
    this->queue = this->device.CreateQueue();
  }

  this->running = true;
  while (this->running && !this->shouldQuit()) {
    this->frame(onRender);
    // TODO(ib): We should not wait 16ms, we should wait **max** 16ms.
    // uSleep(16000);
  }
  this->running = false;
}

void AnimationLoop::stop() { this->running = false; }

/*
  if (!this->onNeedsRedraw(this)) {
    return;
  }
  this->onBeforeRender(this);
  this->onRender(this, pass);
  this->onAfterRender(this);
*/

void AnimationLoop::frame(std::function<void(wgpu::RenderPassEncoder)> onRender) {
  wgpu::TextureView backbufferView = this->swapchain.GetCurrentTextureView();

  utils::ComboRenderPassDescriptor renderPass({backbufferView});
  wgpu::CommandEncoder encoder = device.CreateCommandEncoder();
  {
    wgpu::RenderPassEncoder pass = encoder.BeginRenderPass(&renderPass);

    onRender(pass);  // TODO(ib): protect with try catch

    pass.EndPass();
  }

  wgpu::CommandBuffer commands = encoder.Finish();

  this->queue.Submit(1, &commands);
  this->swapchain.Present();

  this->flush();
}
