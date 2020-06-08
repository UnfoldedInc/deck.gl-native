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

#include "./metal-animation-loop.h"  // NOLINT(build/include)

#import <MetalKit/MTKView.h>

#include "probe.gl/core.h"

using namespace lumagl;

// Animation loop retains the view being passed
MetalAnimationLoop::MetalAnimationLoop(const Options& options) : AnimationLoop{options}, _view{[options.view retain]} {
  // Create a device and queue if none were provided
  auto _device = options.device ? options.device : this->_createDevice();
  auto _queue = options.queue ? options.queue : _device.CreateQueue();

  this->_binding = std::make_unique<util::MetalBinding>(this->_view, _device);
  this->_swapchain = this->_createSwapchain(_device);
  this->_initialize(_device, _queue);
}

MetalAnimationLoop::~MetalAnimationLoop() { [this->_view release]; }

void MetalAnimationLoop::draw(std::function<void(wgpu::RenderPassEncoder)> onRender) {
  wgpu::TextureView backbufferView = this->_swapchain.GetCurrentTextureView();
  super::draw(backbufferView, onRender);
  this->_swapchain.Present();
}

auto MetalAnimationLoop::getPreferredSwapChainTextureFormat() -> wgpu::TextureFormat {
  return static_cast<wgpu::TextureFormat>(this->_binding->GetPreferredSwapChainTextureFormat());
}

auto MetalAnimationLoop::devicePixelRatio() -> float {
  // TODO(ilija@unfolded.ai): Not necessarily the device pixel ratio, but rather scale being used by the layer currently
  // In order to get the device ratio, we'd need a NSWindow/UIWindow
  // Make sure UI code is called on the main thread

  if ([NSThread isMainThread]) {
    return this->_view.layer.contentsScale;
  } else {
    __block float pixelRatio = 1.0;
    dispatch_sync(dispatch_get_main_queue(), ^{
      pixelRatio = this->_view.layer.contentsScale;
    });

    return pixelRatio;
  }
}

void MetalAnimationLoop::setSize(const Size& size) {
  bool sizeChanged = size != this->_size;
  if (sizeChanged) {
    this->_swapchain.Configure(this->getPreferredSwapChainTextureFormat(), wgpu::TextureUsage::OutputAttachment,
                               size.width, size.height);
  }

  super::setSize(size);
}

auto MetalAnimationLoop::_createDevice() -> wgpu::Device {
  this->_instance = std::make_unique<dawn_native::Instance>();
  this->_instance->DiscoverDefaultAdapters();

  // Get an adapter for the backend to use, and create the device.
  dawn_native::Adapter backendAdapter;
  std::vector<dawn_native::Adapter> adapters = this->_instance->GetAdapters();
  auto adapterIt = std::find_if(adapters.begin(), adapters.end(), [=](const dawn_native::Adapter adapter) -> bool {
    wgpu::AdapterProperties properties;
    adapter.GetProperties(&properties);
    return properties.backendType == wgpu::BackendType::Metal;
  });
  ASSERT(adapterIt != adapters.end());
  backendAdapter = *adapterIt;

  WGPUDevice cDevice = backendAdapter.CreateDevice();
  return wgpu::Device::Acquire(cDevice);
}

auto MetalAnimationLoop::_createSwapchain(wgpu::Device device) -> wgpu::SwapChain {
  wgpu::SwapChainDescriptor swapChainDesc;
  swapChainDesc.implementation = this->_binding->GetSwapChainImplementation();
  auto swapchain = device.CreateSwapChain(nullptr, &swapChainDesc);
  swapchain.Configure(this->getPreferredSwapChainTextureFormat(), wgpu::TextureUsage::OutputAttachment,
                      this->_size.width, this->_size.height);

  return swapchain;
}

MetalAnimationLoop::Options::Options(MTKView* view, const wgpu::Device& device, const wgpu::Queue& queue)
    : AnimationLoop::Options{device,
                             queue,
                             {static_cast<int>(view.bounds.size.width), static_cast<int>(view.bounds.size.height)}},
      view{view} {}
