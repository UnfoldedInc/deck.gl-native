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

// Helpers

enum class CmdBufType {
  None,
  Terrible,
  // TODO(cwallez@chromium.org) double terrible cmdbuf
};

// Helpers
static void initializeGLFW(wgpu::BackendType);
auto createCppDawnDevice(wgpu::BackendType) -> wgpu::Device;
uint64_t getSwapChainImplementation();
auto getPreferredSwapChainTextureFormat() -> wgpu::TextureFormat;
// Not used?
auto getSwapChain(const wgpu::Device& device) -> wgpu::SwapChain;
auto createDefaultDepthStencilView(const wgpu::Device& device) -> wgpu::TextureView;

// GLFWAnimationLoop

GLFWAnimationLoop::GLFWAnimationLoop(wgpu::BackendType backendType) : AnimationLoop{} {
  initializeGLFW(backendType);
  this->window = glfwCreateWindow(640, 480, "deck.gl window", nullptr, nullptr);
  if (!this->window) {
    throw new std::runtime_error("Failed to create GLFW window");
  }
}

auto GLFWAnimationLoop::createDevice(wgpu::BackendType backendType) -> wgpu::Device {
  return nullptr;  // createDevice(backendType);
}

bool GLFWAnimationLoop::shouldQuit() { return glfwWindowShouldClose(this->window); }

// void GLFWAnimationLoop::flush() {
//   // if (this->c2sBuf) {
//   //   bool c2sSuccess = c2sBuf->Flush();
//   //   ASSERT(c2sSuccess && s2cSuccess);
//   // }
//   // if (this->s2cBuf) {
//   //   bool s2cSuccess = s2cBuf->Flush();
//   //   ASSERT(c2sSuccess && s2cSuccess);
//   // }
//   glfwPollEvents();
// }

void GLFWAnimationLoop::flush() {
  
  if (c2sBuf) {
    bool c2sSuccess = c2sBuf->Flush();
    // ASSERT(c2sSuccess);
  }
  if (s2cBuf) {
    bool s2cSuccess = s2cBuf->Flush();
    // ASSERT(s2cSuccess);
  }

  glfwPollEvents();
}


GLFWwindow* window{nullptr};
lumagl::utils::TerribleCommandBuffer* c2sBuf{nullptr};
lumagl::utils::TerribleCommandBuffer* s2cBuf{nullptr};


uint64_t getSwapChainImplementation() { return binding->GetSwapChainImplementation(); }

wgpu::TextureFormat getPreferredSwapChainTextureFormat() {
  DoFlush();
  return static_cast<wgpu::TextureFormat>(binding->getPreferredSwapChainTextureFormat());
}

wgpu::SwapChain getSwapChain(const wgpu::Device& device) {
  wgpu::SwapChainDescriptor swapChainDesc;
  swapChainDesc.implementation = getSwapChainImplementation();
  return device.CreateSwapChain(nullptr, &swapChainDesc);
}

wgpu::TextureView createDefaultDepthStencilView(const wgpu::Device& device) {
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

/// \brief initialize glwf library
static void initializeGLFW(wgpu::BackendType backendType) {
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

// TODO(ib): restore or delete

static CmdBufType cmdBufType = CmdBufType::Terrible;
static std::unique_ptr<dawn_native::Instance> instance;
static lumagl::utils::BackendBinding* binding = nullptr;


static dawn_wire::WireServer* wireServer = nullptr;
static dawn_wire::WireClient* wireClient = nullptr;
static lumagl::utils::TerribleCommandBuffer* c2sBuf = nullptr;
static lumagl::utils::TerribleCommandBuffer* s2cBuf = nullptr;

auto createCppDawnDevice(wgpu::BackendType backendType) -> wgpu::Device {

  instance = std::make_unique<dawn_native::Instance>();
  lumagl::utils::DiscoverAdapter(instance.get(), window, backendType);

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

  binding = lumagl::utils::CreateBinding(backendType, window, backendDevice);
  if (binding == nullptr) {
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
      c2sBuf = new lumagl::utils::TerribleCommandBuffer();
      s2cBuf = new lumagl::utils::TerribleCommandBuffer();

      dawn_wire::WireServerDescriptor serverDesc = {};
      serverDesc.device = backendDevice;
      serverDesc.procs = &backendProcs;
      serverDesc.serializer = s2cBuf;

      wireServer = new dawn_wire::WireServer(serverDesc);
      c2sBuf->SetHandler(wireServer);

      dawn_wire::WireClientDescriptor clientDesc = {};
      clientDesc.serializer = c2sBuf;

      wireClient = new dawn_wire::WireClient(clientDesc);
      WGPUDevice clientDevice = wireClient->GetDevice();
      DawnProcTable clientProcs = dawn_wire::WireClient::GetProcs();
      s2cBuf->SetHandler(wireClient);

      procs = clientProcs;
      cDevice = clientDevice;
    } break;
  }

  dawnProcSetProcs(&procs);
  procs.deviceSetUncapturedErrorCallback(
      cDevice,
      [](WGPUErrorType errorType, const char* message, void*) {
        // dawn::ErrorLog() << getDeviceErrorTypeName(errorType) << " error: " << message;
      },
      nullptr);
  wgpu::Device::Acquire(cDevice);
  return cDevice;
}
