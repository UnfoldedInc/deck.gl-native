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

#ifndef LUMAGL_WEBGPU_SWAPCHAINUTILS_H_
#define LUMAGL_WEBGPU_SWAPCHAINUTILS_H_

#include "dawn/dawn_wsi.h"

template <typename T>
DawnSwapChainImplementation CreateSwapChainImplementation(T* swapChain) {
  DawnSwapChainImplementation impl = {};
  impl.userData = swapChain;
  impl.Init = [](void* userData, void* wsiContext) {
    auto* ctx = static_cast<typename T::WSIContext*>(wsiContext);
    reinterpret_cast<T*>(userData)->Init(ctx);
  };
  impl.Destroy = [](void* userData) { delete reinterpret_cast<T*>(userData); };
  impl.Configure = [](void* userData, WGPUTextureFormat format, WGPUTextureUsage allowedUsage, uint32_t width,
                      uint32_t height) {
    return static_cast<T*>(userData)->Configure(format, allowedUsage, width, height);
  };
  impl.GetNextTexture = [](void* userData, DawnSwapChainNextTexture* nextTexture) {
    return static_cast<T*>(userData)->GetNextTexture(nextTexture);
  };
  impl.Present = [](void* userData) { return static_cast<T*>(userData)->Present(); };
  return impl;
}

#endif  // LUMAGL_WEBGPU_SWAPCHAINUTILS_H_
