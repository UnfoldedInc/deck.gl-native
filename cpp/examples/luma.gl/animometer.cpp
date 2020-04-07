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

#include <cstdio>
#include <cstdlib>
#include <vector>

#include "luma.gl/core.h"
#include "probe.gl/core.h"

wgpu::Device device;
wgpu::Queue queue;
wgpu::SwapChain swapchain;
wgpu::RenderPipeline pipeline;
wgpu::BindGroup bindGroup;
wgpu::Buffer ubo;

lumagl::GLFWAnimationLoop animationLoop{wgpu::BackendType::Metal};

float RandomFloat(float min, float max) {
  float zeroOne = rand() / float(RAND_MAX);
  return zeroOne * (max - min) + min;
}

constexpr size_t kNumTriangles = 10000;

struct alignas(lumagl::utils::kMinDynamicBufferOffsetAlignment) ShaderData {
  float scale;
  float time;
  float offsetX;
  float offsetY;
  float scalar;
  float scalarOffset;
};

static std::vector<ShaderData> shaderData;

void init() {
  device = animationLoop.createDevice(wgpu::BackendType::Metal);

  queue = device.CreateQueue();
  swapchain = animationLoop.getSwapChain(device);
  swapchain.Configure(animationLoop.getPreferredSwapChainTextureFormat(), wgpu::TextureUsage::OutputAttachment, 640,
                      480);

  wgpu::ShaderModule vsModule = lumagl::utils::createShaderModule(device, lumagl::utils::SingleShaderStage::Vertex, R"(
        #version 450

        layout(std140, set = 0, binding = 0) uniform Constants {
            float scale;
            float time;
            float offsetX;
            float offsetY;
            float scalar;
            float scalarOffset;
        } c;

        layout(location = 0) out vec4 v_color;

        const vec4 positions[3] = vec4[3](
            vec4( 0.0f,  0.1f, 0.0f, 1.0f),
            vec4(-0.1f, -0.1f, 0.0f, 1.0f),
            vec4( 0.1f, -0.1f, 0.0f, 1.0f)
        );

        const vec4 colors[3] = vec4[3](
            vec4(1.0f, 0.0f, 0.0f, 1.0f),
            vec4(0.0f, 1.0f, 0.0f, 1.0f),
            vec4(0.0f, 0.0f, 1.0f, 1.0f)
        );

        void main() {
            vec4 position = positions[gl_VertexIndex];
            vec4 color = colors[gl_VertexIndex];

            float fade = mod(c.scalarOffset + c.time * c.scalar / 10.0, 1.0);
            if (fade < 0.5) {
                fade = fade * 2.0;
            } else {
                fade = (1.0 - fade) * 2.0;
            }
            float xpos = position.x * c.scale;
            float ypos = position.y * c.scale;
            float angle = 3.14159 * 2.0 * fade;
            float xrot = xpos * cos(angle) - ypos * sin(angle);
            float yrot = xpos * sin(angle) + ypos * cos(angle);
            xpos = xrot + c.offsetX;
            ypos = yrot + c.offsetY;
            v_color = vec4(fade, 1.0 - fade, 0.0, 1.0) + color;
            gl_Position = vec4(xpos, ypos, 0.0, 1.0);
        })");

  wgpu::ShaderModule fsModule =
      lumagl::utils::createShaderModule(device, lumagl::utils::SingleShaderStage::Fragment, R"(
        #version 450
        layout(location = 0) out vec4 fragColor;
        layout(location = 0) in vec4 v_color;
        void main() {
            fragColor = v_color;
        })");

  wgpu::BindGroupLayout bgl = lumagl::utils::makeBindGroupLayout(
      device, {{0, wgpu::ShaderStage::Vertex, wgpu::BindingType::UniformBuffer, true}});

  lumagl::utils::ComboRenderPipelineDescriptor descriptor(device);
  descriptor.layout = lumagl::utils::makeBasicPipelineLayout(device, &bgl);
  descriptor.vertexStage.module = vsModule;
  descriptor.cFragmentStage.module = fsModule;
  descriptor.cColorStates[0].format = animationLoop.getPreferredSwapChainTextureFormat();

  pipeline = device.CreateRenderPipeline(&descriptor);

  shaderData.resize(kNumTriangles);
  for (auto& data : shaderData) {
    data.scale = RandomFloat(0.2f, 0.4f);
    data.time = 0.0;
    data.offsetX = RandomFloat(-0.9f, 0.9f);
    data.offsetY = RandomFloat(-0.9f, 0.9f);
    data.scalar = RandomFloat(0.5f, 2.0f);
    data.scalarOffset = RandomFloat(0.0f, 10.0f);
  }

  wgpu::BufferDescriptor bufferDesc;
  bufferDesc.size = kNumTriangles * sizeof(ShaderData);
  bufferDesc.usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Uniform;
  ubo = device.CreateBuffer(&bufferDesc);

  bindGroup = lumagl::utils::makeBindGroup(device, bgl, {{0, ubo, 0, sizeof(ShaderData)}});
}

void frame() {
  wgpu::TextureView backbufferView = swapchain.GetCurrentTextureView();

  static int f = 0;
  f++;
  for (auto& data : shaderData) {
    data.time = f / 60.0f;
  }
  ubo.SetSubData(0, kNumTriangles * sizeof(ShaderData), shaderData.data());

  lumagl::utils::ComboRenderPassDescriptor renderPass({backbufferView});
  wgpu::CommandEncoder encoder = device.CreateCommandEncoder();
  {
    wgpu::RenderPassEncoder pass = encoder.BeginRenderPass(&renderPass);
    pass.SetPipeline(pipeline);

    for (size_t i = 0; i < kNumTriangles; i++) {
      uint32_t offset = static_cast<uint32_t>(i * sizeof(ShaderData));
      pass.SetBindGroup(0, bindGroup, 1, &offset);
      pass.Draw(3, 1, 0, 0);
    }

    pass.EndPass();
  }

  wgpu::CommandBuffer commands = encoder.Finish();
  queue.Submit(1, &commands);
  swapchain.Present();
  animationLoop.flush();
  //  fprintf(stderr, "frame %i\n", f);
}

int main(int argc, const char* argv[]) {
  init();

  while (!glfwWindowShouldClose(animationLoop.window)) {
    frame();
    probegl::uSleep(16000);
  }

  // TODO release stuff
}

/*

#include "luma.gl/core.h"

using namespace lumagl;
using namespace lumagl::utils;

float RandomFloat(float min, float max) {
  float zeroOne = rand() / float(RAND_MAX);
  return zeroOne * (max - min) + min;
}

constexpr size_t kNumTriangles = 10000;

struct alignas(kMinDynamicBufferOffsetAlignment) ShaderData {
    float scale;
    float time;
    float offsetX;
    float offsetY;
    float scalar;
    float scalarOffset;
};

auto vs = R"(
#version 450

layout(std140, set = 0, binding = 0) uniform Constants {
    float scale;
    float time;
    float offsetX;
    float offsetY;
    float scalar;
    float scalarOffset;
} c;

layout(location = 0) out vec4 v_color;

const vec4 positions[3] = vec4[3](
    vec4( 0.0f,  0.1f, 0.0f, 1.0f),
    vec4(-0.1f, -0.1f, 0.0f, 1.0f),
    vec4( 0.1f, -0.1f, 0.0f, 1.0f)
);

const vec4 colors[3] = vec4[3](
    vec4(1.0f, 0.0f, 0.0f, 1.0f),
    vec4(0.0f, 1.0f, 0.0f, 1.0f),
    vec4(0.0f, 0.0f, 1.0f, 1.0f)
);

void main() {
    vec4 position = positions[gl_VertexIndex];
    vec4 color = colors[gl_VertexIndex];

    float fade = mod(c.scalarOffset + c.time * c.scalar / 10.0, 1.0);
    if (fade < 0.5) {
        fade = fade * 2.0;
    } else {
        fade = (1.0 - fade) * 2.0;
    }
    float xpos = position.x * c.scale;
    float ypos = position.y * c.scale;
    float angle = 3.14159 * 2.0 * fade;
    float xrot = xpos * cos(angle) - ypos * sin(angle);
    float yrot = xpos * sin(angle) + ypos * cos(angle);
    xpos = xrot + c.offsetX;
    ypos = yrot + c.offsetY;
    v_color = vec4(fade, 1.0 - fade, 0.0, 1.0) + color;
    gl_Position = vec4(xpos, ypos, 0.0, 1.0);
})";

auto fs = R"(
#version 450
layout(location = 0) out vec4 fragColor;
layout(location = 0) in vec4 v_color;
void main() {
    fragColor = v_color;
})";

static std::vector<ShaderData> shaderData;

int main(int argc, const char* argv[]) {
  // if (!InitSample(argc, argv)) {
  //     return 1;
  // }

  GLFWAnimationLoop animationLoop{wgpu::BackendType::Vulkan};
  auto device = animationLoop.createDevice(wgpu::BackendType::Vulkan);

//  animationLoop.initializeDevice();

  Model::Options options{vs, fs};
  Model model{device, options};

//  init();

  animationLoop.onBeforeRender = [](AnimationLoop*) {
//    static int f = 0;
//    f++;
//    for (auto& data : shaderData) {
//        data.time = f / 60.0f;
//    }
//    ubo.SetSubData(0, kNumTriangles * sizeof(ShaderData), shaderData.data());
  };

  animationLoop.run([model](wgpu::RenderPassEncoder pass) {
    static int f = 0;
    f++;
    for (auto& data : shaderData) {
        data.time = f / 60.0f;
    }
    ubo.SetSubData(0, kNumTriangles * sizeof(ShaderData), shaderData.data());

    pass.SetPipeline(model.pipeline);
    for (size_t i = 0; i < kNumTriangles; i++) {
      uint32_t offset = i * sizeof(ShaderData);
      pass.SetBindGroup(0, bindGroup, 1, &offset);
      pass.Draw(3, 1, 0, 0);
    }
  });

  // TODO(ib@unfolded.ai): do we need to release stuff?
}


bool InitSample(int argc, const char** argv) {
    for (int i = 1; i < argc; i++) {
        if (std::string("-b") == argv[i] || std::string("--backend") == argv[i]) {
          i++;
          if (i >= argc) {

          }
          auto backendType = i < argc getBackendTypeFromString()
          fprintf(stderr, "--backend expects a backend name (opengl, metal, d3d12, null, vulkan)\n");
          return false;
        }

        cmdBufType = CmdBufType::None;
        if (std::string("-c") == argv[i] || std::string("--command-buffer") == argv[i]) {
            i++;
            if (i < argc && std::string("none") == argv[i]) {
                cmdBufType = CmdBufType::None;
                continue;
            }
            if (i < argc && std::string("terrible") == argv[i]) {
                cmdBufType = CmdBufType::Terrible;
                continue;
            }
            fprintf(stderr, "--command-buffer expects a command buffer name (none, terrible)\n");
            return false;
        }

        if (std::string("-h") == argv[i] || std::string("--help") == argv[i]) {
            printf("Usage: %s [-b BACKEND] [-c COMMAND_BUFFER]\n", argv[0]);
            printf("  BACKEND is one of: d3d12, metal, null, opengl, vulkan\n");
            printf("  COMMAND_BUFFER is one of: none, terrible\n");
            return false;
        }
    }
    return true;
}
*/
