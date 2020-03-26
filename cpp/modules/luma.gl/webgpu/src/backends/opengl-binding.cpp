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

#include "./backend-binding.h"

#include "./swap-chain-utils.h"

#include "dawn/dawn_wsi.h"
#include "dawn_native/OpenGLBackend.h"

#include <cstdio>
#include "GLFW/glfw3.h"

namespace utils {

    class OpenGLBinding : public BackendBinding {
      public:
        OpenGLBinding(GLFWwindow* window, WGPUDevice device) : BackendBinding(window, device) {
        }

        uint64_t GetSwapChainImplementation() override {
            if (mSwapchainImpl.userData == nullptr) {
                mSwapchainImpl = dawn_native::opengl::CreateNativeSwapChainImpl(
                    mDevice,
                    [](void* userdata) { glfwSwapBuffers(static_cast<GLFWwindow*>(userdata)); },
                    mWindow);
            }
            return reinterpret_cast<uint64_t>(&mSwapchainImpl);
        }

        WGPUTextureFormat GetPreferredSwapChainTextureFormat() override {
            return dawn_native::opengl::GetNativeSwapChainPreferredFormat(&mSwapchainImpl);
        }

      private:
        DawnSwapChainImplementation mSwapchainImpl = {};
    };

    BackendBinding* CreateOpenGLBinding(GLFWwindow* window, WGPUDevice device) {
        return new OpenGLBinding(window, device);
    }

}  // namespace utils
