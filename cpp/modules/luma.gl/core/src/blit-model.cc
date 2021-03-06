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

#include "./blit-model.h"  // NOLINT(build/include)

#include <memory>
#include <vector>

#include "math.gl/core.h"

using namespace lumagl;
using namespace mathgl;

namespace {

static auto vs = R"(
#version 450

layout(location = 0) in vec4 pos;

void main() {
  gl_Position = pos;
}
)";

static auto fs = R"(
#version 450

layout(set = 0, binding = 0) uniform sampler textureSampler;
layout(set = 0, binding = 1) uniform texture2D blitTexture;
layout(std140, set = 0, binding = 2) uniform TextureOptions {
  ivec2 size;
} textureOptions;

layout(location = 0) out vec4 fragColor;

void main() {
  fragColor = texture(sampler2D(blitTexture, textureSampler), gl_FragCoord.xy / textureOptions.size);
}
)";

static auto attributeSchema = std::make_shared<garrow::Schema>(
    std::vector<std::shared_ptr<garrow::Field>>{std::make_shared<garrow::Field>("pos", wgpu::VertexFormat::Float2)});

static auto getOptions(const wgpu::Device& device, const Size& textureSize) -> Model::Options {
  auto instancedSchema = std::make_shared<garrow::Schema>(std::vector<std::shared_ptr<garrow::Field>>{});

  return Model::Options{vs,
                        fs,
                        attributeSchema,
                        instancedSchema,
                        {UniformDescriptor{wgpu::ShaderStage::Fragment, wgpu::BindingType::Sampler},
                         UniformDescriptor{wgpu::ShaderStage::Fragment, wgpu::BindingType::SampledTexture},
                         UniformDescriptor{wgpu::ShaderStage::Fragment, wgpu::BindingType::UniformBuffer}},
                        wgpu::PrimitiveTopology::TriangleStrip};
}

};  // anonymous namespace

BlitModel::BlitModel(const wgpu::Device& device, const wgpu::TextureView& textureView, const Size& textureSize)
    : Model{device, getOptions(device, textureSize)} {
  auto samplerDesc = utils::getDefaultSamplerDescriptor();
  auto sampler = device.CreateSampler(&samplerDesc);

  std::vector<std::shared_ptr<garrow::Array>> attribureArrays{std::make_shared<garrow::Array>(
      device, std::vector<Vector2<float>>{{-1, -1}, {1, -1}, {-1, 1}, {1, 1}}, wgpu::BufferUsage::Vertex)};

  auto attributes = std::make_shared<garrow::Table>(attributeSchema, attribureArrays);

  this->setAttributes(attributes);
  this->setUniformSampler(0, sampler);
  this->setUniformTexture(1, textureView);
  this->setTextureSize(textureSize);
}

void BlitModel::setTextureSize(const Size& textureSize) {
  this->setUniformBuffer(2, this->_uniformBufferFromSize(textureSize));
}

auto BlitModel::_uniformBufferFromSize(const Size& size) -> wgpu::Buffer {
  // Using Vector2 when creating uniform buffers to stay consistent, Size structure is subject to change
  auto sizeVector = mathgl::Vector2<int>{size.width, size.height};
  return utils::createBufferFromData(this->device(), &sizeVector, sizeof(sizeVector), wgpu::BufferUsage::Uniform);
}
