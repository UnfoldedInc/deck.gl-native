// Ported from dawn examples

#include "luma.gl/core"

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

int main(int argc, const char* argv[]) {

  // if (!InitSample(argc, argv)) {
  //     return 1;
  // }

  GLFWAnimationLoop animationLoop;

  animationLoop.initializeDevice();

  let model = new Model(animationLoop.device, vs, fs)

  init();

  animationLoop.onBeforeRender = [](AnimationLoop*) {
    static int f = 0;
    f++;
    for (auto& data : shaderData) {
        data.time = f / 60.0f;
    }
    ubo.SetSubData(0, kNumTriangles * sizeof(ShaderData), shaderData.data());
  }

  animationLoop.run([](AnimationLoop*, wgpu::RenderPassEncoder pass) {
    pass.SetPipeline(model.pipeline);
    for (size_t i = 0; i < kNumTriangles; i++) {
      uint32_t offset = i * sizeof(ShaderData);
      pass.SetBindGroup(0, bindGroup, 1, &offset);
      pass.Draw(3, 1, 0, 0);
    }
  });

  // TODO(ib): do we need to release stuff?
}
