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

AnimationLoop::AnimationLoop(wgpu::Device device, wgpu::Queue queue, const Size& size) : _size{size} {
  this->_initialize(device, queue);
}

AnimationLoop::~AnimationLoop() {
  // TODO(ilija@unfolded.ai): Cleanup?
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

void AnimationLoop::frame(wgpu::TextureView textureView, std::function<void(wgpu::RenderPassEncoder)> onRender) {
  // TODO(ilija@unfolded.ai): There seems to be a memory leak, what do we need to free?
  utils::ComboRenderPassDescriptor passDescriptor({textureView});
  wgpu::CommandEncoder encoder = this->_device.CreateCommandEncoder();
  {
    wgpu::RenderPassEncoder pass = encoder.BeginRenderPass(&passDescriptor);

    try {
      onRender(pass);
    } catch (const std::runtime_error& re) {
      // Speciffic handling for runtime_error
      probegl::ErrorLog() << "Drawing failed with untime error: " << re.what();
    } catch (const std::exception& ex) {
      // Speciffic handling for all exceptions extending std::exception, except
      // std::runtime_error which is handled explicitly
      probegl::ErrorLog() << "Drawing failed with exception: " << ex.what();
    } catch (...) {
      // Catch any other errors (that we have no information about)
      probegl::ErrorLog() << "Unknown failure occurred during drawing. Possible memory corruption";
    }

    pass.EndPass();
  }

  wgpu::CommandBuffer commands = encoder.Finish();
  this->_queue.Submit(1, &commands);

  this->flush();
}

void AnimationLoop::stop() { this->running = false; }

void AnimationLoop::setSize(const Size& size) {
  bool sizeChanged = size.width != this->_size.width || size.height != this->_size.height;
  if (sizeChanged) {
    this->_size = size;
    // TODO(ilija@unfolded.ai): Trigger redraw
  }
}

void AnimationLoop::_initialize(wgpu::Device device, wgpu::Queue queue) {
  dawn_native::GetProcs().deviceSetUncapturedErrorCallback(
      device.Get(),
      [](WGPUErrorType errorType, const char* message, void*) {
        probegl::ErrorLog() << getWebGPUErrorName(errorType) << " error: " << message;
      },
      nullptr);

  this->_device = device;
  this->_queue = queue ? queue : device.CreateQueue();
}
