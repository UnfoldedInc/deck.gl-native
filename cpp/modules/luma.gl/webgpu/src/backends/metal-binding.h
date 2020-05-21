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

#ifndef LUMAGL_WEBGPU_BACKENDS_METAL_BINDING_H
#define LUMAGL_WEBGPU_BACKENDS_METAL_BINDING_H

#include <dawn/dawn_wsi.h>
#include <dawn/webgpu_cpp.h>
#include <dawn_native/DawnNative.h>

#ifdef __OBJC__
@class MTKView;
#else
using MTKView = void*;
#endif

namespace lumagl {
namespace util {

class MetalBinding {
 public:
  MetalBinding(MTKView* view, const wgpu::Device device) : _view{view}, _device{device} {}

  auto GetSwapChainImplementation() -> uint64_t;
  WGPUTextureFormat GetPreferredSwapChainTextureFormat() { return WGPUTextureFormat_BGRA8Unorm; }

 private:
  DawnSwapChainImplementation _swapchainImpl = {};

  MTKView* _view;
  wgpu::Device _device;
};

}  // namespace util
}  // namespace lumagl

#endif  // LUMAGL_WEBGPU_BACKENDS_METAL_BINDING_H
