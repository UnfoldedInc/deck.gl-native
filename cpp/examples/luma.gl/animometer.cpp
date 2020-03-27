// Ported from dawn examples

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
    static int f = 0;
    f++;
    for (auto& data : shaderData) {
        data.time = f / 60.0f;
    }
    ubo.SetSubData(0, kNumTriangles * sizeof(ShaderData), shaderData.data());
  }

  animationLoop.run([](wgpu::RenderPassEncoder pass) {
    pass.SetPipeline(model.pipeline);
    for (size_t i = 0; i < kNumTriangles; i++) {
      uint32_t offset = i * sizeof(ShaderData);
      pass.SetBindGroup(0, bindGroup, 1, &offset);
      pass.Draw(3, 1, 0, 0);
    }
  });

  // TODO(ib@unfolded.ai): do we need to release stuff?
}


/*
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
