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

#include <vector>

#include "luma.gl/core.h"
#include "math.gl/core.h"
#include "probe.gl/core.h"

using namespace lumagl;
using namespace mathgl;

namespace {

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

layout(location = 0) in vec4 positions;
layout(location = 1) in vec4 colors;

void main() {
    vec4 position = positions;
    vec4 color = colors;

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
layout(location = 0) in vec4 v_color;
layout(location = 0) out vec4 fragColor;
void main() {
    fragColor = v_color;
})";

auto randomFloat(float min, float max) -> float {
  float zeroOne = rand() / float(RAND_MAX);
  return zeroOne * (max - min) + min;
}

constexpr size_t kNumTriangles = 1000;

struct alignas(utils::kMinDynamicBufferOffsetAlignment) ShaderData {
  float scale;
  float time;
  float offsetX;
  float offsetY;
  float scalar;
  float scalarOffset;
};

auto createSampleData(int count) -> std::vector<ShaderData> {
  std::vector<ShaderData> shaderData;
  shaderData.resize(count);
  for (auto& data : shaderData) {
    data.scale = randomFloat(0.2f, 0.4f);
    data.time = 0.0;
    data.offsetX = randomFloat(-0.9f, 0.9f);
    data.offsetY = randomFloat(-0.9f, 0.9f);
    data.scalar = randomFloat(0.5f, 2.0f);
    data.scalarOffset = randomFloat(0.0f, 10.0f);
  }

  return shaderData;
}

auto createAttributeTable(wgpu::Device device) -> std::shared_ptr<garrow::Table> {
  auto schema = std::make_shared<garrow::Schema>(std::vector<std::shared_ptr<garrow::Field>>{
      std::make_shared<garrow::Field>("positions", wgpu::VertexFormat::Float4),
      std::make_shared<garrow::Field>("colors", wgpu::VertexFormat::Float4)});

  auto positionsArray = std::make_shared<garrow::Array>(
      device,
      std::vector<Vector4<float>>{{0.0f, 0.1f, 0.0f, 1.0f}, {-0.1f, -0.1f, 0.0f, 1.0f}, {0.1f, -0.1f, 0.0f, 1.0f}});

  auto colorsArray = std::make_shared<garrow::Array>(
      device,
      std::vector<Vector4<float>>{{1.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f, 1.0f}});
  std::vector<std::shared_ptr<garrow::Array>> arrays{positionsArray, colorsArray};

  return std::make_shared<garrow::Table>(schema, arrays);
}

}  // anonymous namespace

int main(int argc, const char* argv[]) {
  GLFWAnimationLoop animationLoop{};
  wgpu::Device device = *(animationLoop.device.get());

  auto attributes = createAttributeTable(device);
  auto instancedSchema = std::make_shared<garrow::Schema>(std::vector<std::shared_ptr<garrow::Field>>{});
  Model model{device, {vs, fs, attributes->schema(), instancedSchema, {{sizeof(ShaderData)}}}};

  std::vector<ShaderData> shaderData = createSampleData(kNumTriangles);
  std::vector<wgpu::Buffer> uniformBuffers;
  for (auto const& data : shaderData) {
    auto ubo = utils::createBufferFromData(device, &data, sizeof(ShaderData), wgpu::BufferUsage::Uniform);
    uniformBuffers.push_back(ubo);
  }

  model.setAttributes(attributes);
  model.vertexCount = 3;

  animationLoop.run([&](wgpu::RenderPassEncoder pass) {
    static int f = 0;
    f++;

    for (auto i = 0; i < uniformBuffers.size(); ++i) {
      // Update buffer
      shaderData[i].time = f / 60.0f;
      uniformBuffers[i].SetSubData(0, sizeof(ShaderData), &(shaderData[i]));

      model.setUniformBuffers({uniformBuffers[i]});
      model.draw(pass);
    }
  });

  return 0;
}
