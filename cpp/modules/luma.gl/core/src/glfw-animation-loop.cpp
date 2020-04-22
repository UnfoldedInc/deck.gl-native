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

#include "./glfw-animation-loop.h"  // NOLINT(build/include)

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <dawn/dawn_wsi.h>
#include <dawn/webgpu_cpp.h>
#include <dawn_native/DawnNative.h>

#include <iostream>

#include "luma.gl/webgpu.h"
#include "probe.gl/core.h"

#if defined(DAWN_PLATFORM_WINDOWS)
#define GLFW_EXPOSE_NATIVE_WIN32
#elif defined(DAWN_USE_X11)
#define GLFW_EXPOSE_NATIVE_X11
#endif

using namespace lumagl;
using namespace lumagl::utils;

GLFWAnimationLoop::GLFWAnimationLoop(const wgpu::BackendType backendType, wgpu::Device device, const Size& size)
    : AnimationLoop{size} {
  this->_window = this->_initializeGLFW(backendType);

  // Create a device if none was provided
  auto _device = device;
  if (device == nullptr) {
    _device = this->_createDevice(backendType);
  }

  this->_binding = CreateBinding(backendType, this->_window, _device.Get());
  if (this->_binding == nullptr) {
    throw new std::runtime_error("Failed to create binding");
  }

  this->_initialize(backendType, _device);
}

GLFWAnimationLoop::~GLFWAnimationLoop() {
  glfwDestroyWindow(this->_window);
  glfwTerminate();

  // TODO(ilija@unfolded.ai): Additional cleanup?
}

void GLFWAnimationLoop::frame(std::function<void(wgpu::RenderPassEncoder)> onRender) {
  // Break the run loop if esc is pressed
  if (glfwGetKey(this->_window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(this->_window, true);
  }

  AnimationLoop::frame(onRender);
}

bool GLFWAnimationLoop::shouldQuit() { return glfwWindowShouldClose(this->_window); }

void GLFWAnimationLoop::flush() { glfwPollEvents(); }

auto GLFWAnimationLoop::getPreferredSwapChainTextureFormat() -> wgpu::TextureFormat {
  // TODO(ilija@unfolded.ai): Why do we flush here?
  this->flush();
  return static_cast<wgpu::TextureFormat>(this->_binding->GetPreferredSwapChainTextureFormat());
}

auto GLFWAnimationLoop::_createDevice(const wgpu::BackendType backendType) -> wgpu::Device {
  this->_instance = std::make_unique<dawn_native::Instance>();
  DiscoverAdapter(this->_instance.get(), this->_window, backendType);

  // Get an adapter for the backend to use, and create the device.
  dawn_native::Adapter backendAdapter;
  {
    std::vector<dawn_native::Adapter> adapters = this->_instance->GetAdapters();
    auto adapterIt = std::find_if(adapters.begin(), adapters.end(), [=](const dawn_native::Adapter adapter) -> bool {
      wgpu::AdapterProperties properties;
      adapter.GetProperties(&properties);
      return properties.backendType == backendType;
    });
    ASSERT(adapterIt != adapters.end());
    backendAdapter = *adapterIt;
  }

  WGPUDevice cDevice = backendAdapter.CreateDevice();
  return wgpu::Device::Acquire(cDevice);
}

auto GLFWAnimationLoop::_createSwapchain(wgpu::Device device) -> wgpu::SwapChain {
  wgpu::SwapChainDescriptor swapChainDesc;
  swapChainDesc.implementation = this->_binding->GetSwapChainImplementation();
  auto swapchain = device.CreateSwapChain(nullptr, &swapChainDesc);
  swapchain.Configure(this->getPreferredSwapChainTextureFormat(), wgpu::TextureUsage::OutputAttachment,
                      this->_size.width, this->_size.height);

  return swapchain;
}

auto GLFWAnimationLoop::_initializeGLFW(const wgpu::BackendType backendType) -> GLFWwindow* {
  // Set up an error logging callback
  glfwSetErrorCallback(
      [](int code, const char* message) { probegl::ErrorLog() << "GLFW error: " << code << " - " << message; });

  // Init the library
  if (!glfwInit()) {
    throw new std::runtime_error("Failed to initialize GLFW");
  }

  // Configure graphics context creation
  switch (backendType) {
    case wgpu::BackendType::OpenGL:
      // Ask for OpenGL 4.4 which is what the GL backend requires for compute shaders and texture views.
      glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
      glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
      glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
      glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
      break;
    default:
      // Without this GLFW will initialize a GL context on the window, which prevents using
      // the window with other APIs (by crashing in weird ways).
      glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  }

  auto window = glfwCreateWindow(this->_size.width, this->_size.height, "deck.gl window", nullptr, nullptr);
  if (!window) {
    throw new std::runtime_error("Failed to create GLFW window");
  }
  // TODO(ilija@unfolded.ai): Handle window resizing
  //  glfwSetFramebufferSizeCallback(window, [this](GLFWwindow* window, int width, int height) {
  //    this->setSize(width, height);
  //  });

  return window;
}
