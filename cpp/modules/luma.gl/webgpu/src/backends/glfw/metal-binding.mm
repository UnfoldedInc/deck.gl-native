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

// Note: This file was inspired by the Dawn codebase at https://dawn.googlesource.com/dawn/
// Copyright 2017 The Dawn Authors http://www.apache.org/licenses/LICENSE-2.0

#import <QuartzCore/CAMetalLayer.h>
#include <dawn/dawn_wsi.h>
#include <dawn_native/MetalBackend.h>

#include "./backend-binding.h"

#define GLFW_EXPOSE_NATIVE_COCOA
// Ignore documentation warnings coming from glfw3.h
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdocumentation"
#include <GLFW/glfw3.h>
#pragma clang diagnostic pop
#include <GLFW/glfw3native.h>

#include "luma.gl/core.h"
#include "probe.gl/core.h"

namespace lumagl {
namespace utils {
namespace glfw {

class SwapChainImplMTL {
 public:
  using WSIContext = DawnWSIContextMetal;

  SwapChainImplMTL(NSWindow* nsWindow) : mNsWindow(nsWindow) {}

  ~SwapChainImplMTL() {
    [mCurrentTexture release];
    [mCurrentDrawable release];
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

    NSView* contentView = [mNsWindow contentView];
    // Make sure display scale factor is taken into account
    CGSize drawableSize = CGSizeMake(width * mNsWindow.backingScaleFactor, height * mNsWindow.backingScaleFactor);

    // If swapchain was not configured previously, create a new layer and set it up
    if (!mLayer) {
      [contentView setWantsLayer:YES];

      mLayer = [CAMetalLayer layer];
      [mLayer setDevice:mMtlDevice];
      [mLayer setPixelFormat:MTLPixelFormatBGRA8Unorm];
    }

    [mLayer setDrawableSize:drawableSize];

    constexpr uint32_t kFramebufferOnlyTextureUsages = WGPUTextureUsage_OutputAttachment | WGPUTextureUsage_Present;
    bool hasOnlyFramebufferUsages = !(usage & (~kFramebufferOnlyTextureUsages));
    if (hasOnlyFramebufferUsages) {
      [mLayer setFramebufferOnly:YES];
    }

    [contentView setLayer:mLayer];

    return DAWN_SWAP_CHAIN_NO_ERROR;
  }

  DawnSwapChainError GetNextTexture(DawnSwapChainNextTexture* nextTexture) {
    [mCurrentDrawable release];
    mCurrentDrawable = [mLayer nextDrawable];
    [mCurrentDrawable retain];

    [mCurrentTexture release];
    mCurrentTexture = mCurrentDrawable.texture;
    [mCurrentTexture retain];

    nextTexture->texture.ptr = reinterpret_cast<void*>(mCurrentTexture);

    return DAWN_SWAP_CHAIN_NO_ERROR;
  }

  DawnSwapChainError Present() {
    id<MTLCommandBuffer> commandBuffer = [mCommandQueue commandBuffer];
    [commandBuffer presentDrawable:mCurrentDrawable];
    [commandBuffer commit];

    return DAWN_SWAP_CHAIN_NO_ERROR;
  }

 private:
  NSWindow* mNsWindow = nullptr;
  id<MTLDevice> mMtlDevice = nil;
  id<MTLCommandQueue> mCommandQueue = nil;

  CAMetalLayer* mLayer = nullptr;
  id<CAMetalDrawable> mCurrentDrawable = nil;
  id<MTLTexture> mCurrentTexture = nil;
};

class MetalBinding : public BackendBinding {
 public:
  MetalBinding(GLFWwindow* window, WGPUDevice device) : BackendBinding(window, device) {}

  uint64_t GetSwapChainImplementation() override {
    if (mSwapchainImpl.userData == nullptr) {
      mSwapchainImpl = CreateSwapChainImplementation(new SwapChainImplMTL(glfwGetCocoaWindow(mWindow)));
    }
    return reinterpret_cast<uint64_t>(&mSwapchainImpl);
  }

  WGPUTextureFormat GetPreferredSwapChainTextureFormat() override { return WGPUTextureFormat_BGRA8Unorm; }

 private:
  DawnSwapChainImplementation mSwapchainImpl = {};
};

BackendBinding* CreateMetalBinding(GLFWwindow* window, WGPUDevice device) { return new MetalBinding(window, device); }

}  // namespace glfw
}  // namespace util
}  // namespace lumagl
