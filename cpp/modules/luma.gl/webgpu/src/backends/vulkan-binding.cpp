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

#include "dawn_native/VulkanBackend.h"

// Include GLFW after VulkanBackend so that it declares the Vulkan-specific functions
#include "GLFW/glfw3.h"

#include <memory>

namespace utils {

    class VulkanBinding : public BackendBinding {
      public:
        VulkanBinding(GLFWwindow* window, WGPUDevice device) : BackendBinding(window, device) {
        }

        uint64_t GetSwapChainImplementation() override {
            if (mSwapchainImpl.userData == nullptr) {
                VkSurfaceKHR surface = VK_NULL_HANDLE;
                if (glfwCreateWindowSurface(dawn_native::vulkan::GetInstance(mDevice), mWindow,
                                            nullptr, &surface) != VK_SUCCESS) {
                    ASSERT(false);
                }

                mSwapchainImpl = dawn_native::vulkan::CreateNativeSwapChainImpl(mDevice, surface);
            }
            return reinterpret_cast<uint64_t>(&mSwapchainImpl);
        }
        WGPUTextureFormat GetPreferredSwapChainTextureFormat() override {
            ASSERT(mSwapchainImpl.userData != nullptr);
            return dawn_native::vulkan::GetNativeSwapChainPreferredFormat(&mSwapchainImpl);
        }

      private:
        DawnSwapChainImplementation mSwapchainImpl = {};
    };

    BackendBinding* CreateVulkanBinding(GLFWwindow* window, WGPUDevice device) {
        return new VulkanBinding(window, device);
    }

}  // namespace utils
