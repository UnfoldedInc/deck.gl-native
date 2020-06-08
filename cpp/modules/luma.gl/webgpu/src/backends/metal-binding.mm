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

#include "metal-binding.h"

#import <MetalKit/MTKView.h>
#import <QuartzCore/CAMetalLayer.h>
#include <dawn_native/MetalBackend.h>

#include "luma.gl/core.h"
#include "probe.gl/core.h"

using namespace lumagl::util;

namespace lumagl {
namespace util {

class SwapChainImplMTL {
 public:
  using WSIContext = DawnWSIContextMetal;

  // Swapchain retains the view being passed
  SwapChainImplMTL(MTKView* view) : _view{[view retain]} {}

  ~SwapChainImplMTL() {
    [mCurrentTexture release];
    [mCurrentDrawable release];
    [_view release];
  }

  void Init(DawnWSIContextMetal* ctx) {
    mMtlDevice = ctx->device;
    mCommandQueue = ctx->queue;
  }

  DawnSwapChainError Configure(WGPUTextureFormat format, WGPUTextureUsage usage, uint32_t width, uint32_t height) {
    if (format != WGPUTextureFormat_BGRA8Unorm) {
      return "Unsupported format";
    }
    ASSERT(width > 0);
    ASSERT(height > 0);

    // Make sure UI methods are called on the main thread
    // No retain cycle here as there is no reference counting in C++
    executeOnMainThread(^{
      // If swapchain was not configured previously, create a new layer and set it up
      if (!this->mLayer) {
        this->_view.device = mMtlDevice;
        this->mLayer = (CAMetalLayer*)this->_view.layer;
      }

      constexpr uint32_t kFramebufferOnlyTextureUsages = WGPUTextureUsage_OutputAttachment | WGPUTextureUsage_Present;
      bool hasOnlyFramebufferUsages = !(usage & (~kFramebufferOnlyTextureUsages));
      if (hasOnlyFramebufferUsages) {
        [this->mLayer setFramebufferOnly:YES];
      }
    });

    return DAWN_SWAP_CHAIN_NO_ERROR;
  }

  DawnSwapChainError GetNextTexture(DawnSwapChainNextTexture* nextTexture) {
    // Make sure UI methods are called on the main thread
    // No retain cycle here as there is no reference counting in C++
    executeOnMainThread(^{
      [mCurrentDrawable release];
      mCurrentDrawable = [mLayer nextDrawable];
      [mCurrentDrawable retain];

      [mCurrentTexture release];
      mCurrentTexture = mCurrentDrawable.texture;
      [mCurrentTexture retain];

      nextTexture->texture.ptr = reinterpret_cast<void*>(mCurrentTexture);
    });

    return DAWN_SWAP_CHAIN_NO_ERROR;
  }

  DawnSwapChainError Present() {
    id<MTLCommandBuffer> commandBuffer = [mCommandQueue commandBuffer];
    [commandBuffer presentDrawable:mCurrentDrawable];
    [commandBuffer commit];

    return DAWN_SWAP_CHAIN_NO_ERROR;
  }

  void executeOnMainThread(void (^block)()) {
    if ([NSThread mainThread]) {
      block();
    } else {
      dispatch_sync(dispatch_get_main_queue(), block);
    }
  }

 private:
  MTKView* _view;
  id<MTLDevice> mMtlDevice = nil;
  id<MTLCommandQueue> mCommandQueue = nil;

  CAMetalLayer* mLayer = nullptr;
  id<CAMetalDrawable> mCurrentDrawable = nil;
  id<MTLTexture> mCurrentTexture = nil;
};

}  // namespace util
}  // namespace lumagl

auto MetalBinding::GetSwapChainImplementation() -> uint64_t {
  if (this->_swapchainImpl.userData == nullptr) {
    this->_swapchainImpl = CreateSwapChainImplementation(new SwapChainImplMTL{this->_view});
  }

  return reinterpret_cast<uint64_t>(&this->_swapchainImpl);
}
