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

#include "./backend-binding.h"  // NOLINT(build/include)

// Ignore documentation warnings coming from glfw3.h
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdocumentation"
#include <GLFW/glfw3.h>
#pragma clang diagnostic pop
#if defined(LUMAGL_ENABLE_BACKEND_OPENGL)
#include <dawn_native/OpenGLBackend.h>
#endif  // defined(LUMAGL_ENABLE_BACKEND_OPENGL)

#include "luma.gl/core.h"
#include "probe.gl/core.h"

namespace lumagl {
namespace utils {
namespace glfw {

#if defined(LUMAGL_ENABLE_BACKEND_D3D12)
BackendBinding* CreateD3D12Binding(GLFWwindow* window, WGPUDevice device);
#endif
#if defined(LUMAGL_ENABLE_BACKEND_METAL)
BackendBinding* CreateMetalBinding(GLFWwindow* window, WGPUDevice device);
#endif
#if defined(LUMAGL_ENABLE_BACKEND_NULL)
BackendBinding* CreateNullBinding(GLFWwindow* window, WGPUDevice device);
#endif
#if defined(LUMAGL_ENABLE_BACKEND_OPENGL)
BackendBinding* CreateOpenGLBinding(GLFWwindow* window, WGPUDevice device);
#endif
#if defined(LUMAGL_ENABLE_BACKEND_VULKAN)
BackendBinding* CreateVulkanBinding(GLFWwindow* window, WGPUDevice device);
#endif

BackendBinding::BackendBinding(GLFWwindow* window, WGPUDevice device) : mWindow(window), mDevice(device) {}

void DiscoverAdapter(dawn_native::Instance* instance, GLFWwindow* window, wgpu::BackendType type) {
  PROBEGL_UNUSED(type);
  PROBEGL_UNUSED(window);

  if (type == wgpu::BackendType::OpenGL) {
#if defined(LUMAGL_ENABLE_BACKEND_OPENGL)
    glfwMakeContextCurrent(window);
    dawn_native::opengl::AdapterDiscoveryOptions adapterOptions;
    adapterOptions.getProc = reinterpret_cast<void* (*)(const char*)>(glfwGetProcAddress);
    instance->DiscoverAdapters(&adapterOptions);
#endif  // defined(LUMAGL_ENABLE_BACKEND_OPENGL)
  } else {
    instance->DiscoverDefaultAdapters();
  }
}

BackendBinding* CreateBinding(wgpu::BackendType type, GLFWwindow* window, WGPUDevice device) {
  switch (type) {
#if defined(LUMAGL_ENABLE_BACKEND_D3D12)
    case wgpu::BackendType::D3D12:
      return CreateD3D12Binding(window, device);
#endif

#if defined(LUMAGL_ENABLE_BACKEND_METAL)
    case wgpu::BackendType::Metal:
      return CreateMetalBinding(window, device);
#endif

#if defined(LUMAGL_ENABLE_BACKEND_NULL)
    case wgpu::BackendType::Null:
      return CreateNullBinding(window, device);
#endif

#if defined(LUMAGL_ENABLE_BACKEND_OPENGL)
    case wgpu::BackendType::OpenGL:
      return CreateOpenGLBinding(window, device);
#endif

#if defined(LUMAGL_ENABLE_BACKEND_VULKAN)
    case wgpu::BackendType::Vulkan:
      return CreateVulkanBinding(window, device);
#endif

    default:
      return nullptr;
  }
}

}  // namespace glfw
}  // namespace utils
}  // namespace lumagl
