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

AnimationLoop::AnimationLoop(const Size& size) : _size{size} {
  // NOTE: This **must** be done before any wgpu API calls as otherwise functions will be undefined
  // TODO(ilija@unfolded.ai): This should likely be set up globally?
  DawnProcTable procs = dawn_native::GetProcs();
  dawnProcSetProcs(&procs);

  this->_procs = procs;
}

AnimationLoop::~AnimationLoop() {
  // TODO(ilija@unfolded.ai): Cleanup?
}

void AnimationLoop::setSize(const Size& size) {
  bool sizeChanged = size.width != this->_size.width || size.height != this->_size.height;
  if (sizeChanged) {
    this->_size = size;
    this->_swapchain = this->_createSwapchain(this->_device);
    // TODO(ilija@unfolded.ai): Trigger redraw
  }
}

void AnimationLoop::run(std::function<void(wgpu::RenderPassEncoder)> onRender) {
  this->running = true;
  // TODO(ilija@unfolded.ai): Add needsRedraw and check it
  while (this->running && !this->shouldQuit()) {
    this->frame(onRender);
    // TODO(ib@unfolded.ai): We should not wait 16ms, we should wait **max** 16ms.
    probegl::uSleep(16000);
  }
  this->running = false;
}

void AnimationLoop::frame(std::function<void(wgpu::RenderPassEncoder)> onRender) {
  // TODO(ilija@unfolded.ai): There seems to be a memory leak, what do we need to free?
  wgpu::TextureView backbufferView = this->_swapchain.GetCurrentTextureView();

  utils::ComboRenderPassDescriptor passDescriptor({backbufferView});
  wgpu::CommandEncoder encoder = this->_device.CreateCommandEncoder();
  {
    wgpu::RenderPassEncoder pass = encoder.BeginRenderPass(&passDescriptor);

    onRender(pass);  // TODO(ib@unfolded.ai): protect with try catch

    pass.EndPass();
  }

  wgpu::CommandBuffer commands = encoder.Finish();

  this->_queue.Submit(1, &commands);
  this->_swapchain.Present();

  this->flush();
}

void AnimationLoop::stop() { this->running = false; }

void AnimationLoop::_initialize(const wgpu::BackendType backendType, wgpu::Device device) {
  this->_procs.deviceSetUncapturedErrorCallback(
      device.Get(),
      [](WGPUErrorType errorType, const char* message, void*) {
        probegl::ErrorLog() << getWebGPUErrorName(errorType) << " error: " << message;
      },
      nullptr);

  this->_device = device;
  this->_queue = this->_device.CreateQueue();
  this->_swapchain = this->_createSwapchain(this->_device);
}
