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

#include <memory>

#include "./backend-binding.h"
#include "dawn_native/NullBackend.h"
#include "probe.gl/core.h"

namespace lumagl {
namespace utils {
namespace glfw {

class NullBinding : public BackendBinding {
 public:
  NullBinding(GLFWwindow* window, WGPUDevice device) : BackendBinding(window, device) {}

  uint64_t GetSwapChainImplementation() override {
    if (mSwapchainImpl.userData == nullptr) {
      mSwapchainImpl = dawn_native::null::CreateNativeSwapChainImpl();
    }
    return reinterpret_cast<uint64_t>(&mSwapchainImpl);
  }
  WGPUTextureFormat GetPreferredSwapChainTextureFormat() override { return WGPUTextureFormat_RGBA8Unorm; }

 private:
  DawnSwapChainImplementation mSwapchainImpl = {};
};

BackendBinding* CreateNullBinding(GLFWwindow* window, WGPUDevice device) { return new NullBinding(window, device); }

}  // namespace glfw
}  // namespace utils
}  // namespace lumagl
