// Copyright 2017 The Dawn Authors under http://www.apache.org/licenses/LICENSE-2.0

#include "./glfw-animation-loop.h"  // NOLINT(build/include)

#include <algorithm>
#include <cstring>

#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"
#include "dawn/dawn_proc.h"
#include "dawn/dawn_wsi.h"
#include "dawn/webgpu_cpp.h"
#include "dawn_native/DawnNative.h"
#include "dawn_wire/WireClient.h"
#include "dawn_wire/WireServer.h"

// #include "utils/BackendBinding.h"
// #include "utils/GLFWUtils.h"
// #include "utils/TerribleCommandBuffer.h"

#include "luma.gl/webgpu.h"
#include "probe.gl/core.h"

#if defined(DAWN_PLATFORM_WINDOWS)
#define GLFW_EXPOSE_NATIVE_WIN32
#elif defined(DAWN_USE_X11)
#define GLFW_EXPOSE_NATIVE_X11
#endif

using namespace lumagl;
using namespace lumagl::utils;

enum class CmdBufType {
  None,
  Terrible,
  // TODO(cwallez@chromium.org) double terrible cmdbuf
};

static auto cmdBufType = CmdBufType::Terrible;

GLFWAnimationLoop::GLFWAnimationLoop(wgpu::BackendType backendType) : AnimationLoop{} {
  this->_initializeGLFW(backendType);
  this->window = glfwCreateWindow(640, 480, "deck.gl window", nullptr, nullptr);
  if (!this->window) {
    throw new std::runtime_error("Failed to create GLFW window");
  }
}

auto GLFWAnimationLoop::createDevice(wgpu::BackendType backendType) -> wgpu::Device {
  return this->_createCppDawnDevice(backendType);
}

bool GLFWAnimationLoop::shouldQuit() { return glfwWindowShouldClose(this->window); }

void GLFWAnimationLoop::flush() {
  if (this->_c2sBuf) {
    bool c2sSuccess = this->_c2sBuf->Flush();
    ASSERT(c2sSuccess);
  }
  if (this->_s2cBuf) {
    bool s2cSuccess = this->_s2cBuf->Flush();
    ASSERT(s2cSuccess);
  }

  glfwPollEvents();
}

auto GLFWAnimationLoop::getSwapChain(const wgpu::Device& device) -> wgpu::SwapChain {
  wgpu::SwapChainDescriptor swapChainDesc;
  swapChainDesc.implementation = this->_getSwapChainImplementation();
  return device.CreateSwapChain(nullptr, &swapChainDesc);
}

wgpu::TextureFormat GLFWAnimationLoop::getPreferredSwapChainTextureFormat() {
  this->flush();
  return static_cast<wgpu::TextureFormat>(this->_binding->GetPreferredSwapChainTextureFormat());
}

void GLFWAnimationLoop::_initializeGLFW(wgpu::BackendType backendType) {
  // Set up an error logging callback
  glfwSetErrorCallback([](int code, const char* message) {
    // dawn::ErrorLog() << "GLFW error: " << code << " - " << message;
  });

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
}

uint64_t GLFWAnimationLoop::_getSwapChainImplementation() { return this->_binding->GetSwapChainImplementation(); }

auto GLFWAnimationLoop::_createCppDawnDevice(wgpu::BackendType backendType) -> wgpu::Device {
  auto instance = std::make_unique<dawn_native::Instance>();
  DiscoverAdapter(instance.get(), this->window, backendType);

  // Get an adapter for the backend to use, and create the device.
  dawn_native::Adapter backendAdapter;
  {
    std::vector<dawn_native::Adapter> adapters = instance->GetAdapters();
    auto adapterIt = std::find_if(adapters.begin(), adapters.end(), [=](const dawn_native::Adapter adapter) -> bool {
      wgpu::AdapterProperties properties;
      adapter.GetProperties(&properties);
      return properties.backendType == backendType;
    });
    ASSERT(adapterIt != adapters.end());
    backendAdapter = *adapterIt;
  }

  WGPUDevice backendDevice = backendAdapter.CreateDevice();
  DawnProcTable backendProcs = dawn_native::GetProcs();

  this->_binding = CreateBinding(backendType, this->window, backendDevice);
  if (this->_binding == nullptr) {
    return nullptr;
  }

  // Choose whether to use the backend procs and devices directly, or set up the wire.
  WGPUDevice cDevice = nullptr;
  DawnProcTable procs;

  switch (cmdBufType) {
    case CmdBufType::None:
      procs = backendProcs;
      cDevice = backendDevice;
      break;

    case CmdBufType::Terrible: {
      this->_c2sBuf = new TerribleCommandBuffer();
      this->_s2cBuf = new TerribleCommandBuffer();

      dawn_wire::WireServerDescriptor serverDesc = {};
      serverDesc.device = backendDevice;
      serverDesc.procs = &backendProcs;
      serverDesc.serializer = this->_s2cBuf;

      this->_wireServer = new dawn_wire::WireServer(serverDesc);
      this->_c2sBuf->SetHandler(this->_wireServer);

      dawn_wire::WireClientDescriptor clientDesc = {};
      clientDesc.serializer = this->_c2sBuf;

      this->_wireClient = new dawn_wire::WireClient(clientDesc);
      WGPUDevice clientDevice = this->_wireClient->GetDevice();
      DawnProcTable clientProcs = dawn_wire::WireClient::GetProcs();
      this->_s2cBuf->SetHandler(this->_wireClient);

      procs = clientProcs;
      cDevice = clientDevice;
    } break;
  }

  dawnProcSetProcs(&procs);
  procs.deviceSetUncapturedErrorCallback(
      cDevice,
      [](WGPUErrorType errorType, const char* message, void*) {
        probegl::ErrorLog() << getWebGPUErrorName(errorType) << " error: " << message;
      },
      nullptr);
  return wgpu::Device::Acquire(cDevice);
}

auto GLFWAnimationLoop::_createDefaultDepthStencilView(const wgpu::Device& device) -> wgpu::TextureView {
  wgpu::TextureDescriptor descriptor;
  descriptor.dimension = wgpu::TextureDimension::e2D;
  descriptor.size.width = 640;
  descriptor.size.height = 480;
  descriptor.size.depth = 1;
  descriptor.arrayLayerCount = 1;
  descriptor.sampleCount = 1;
  descriptor.format = wgpu::TextureFormat::Depth24PlusStencil8;
  descriptor.mipLevelCount = 1;
  descriptor.usage = wgpu::TextureUsage::OutputAttachment;
  auto depthStencilTexture = device.CreateTexture(&descriptor);
  return depthStencilTexture.CreateView();
}
