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

#ifndef LUMAGL_CORE_MODEL_H
#define LUMAGL_CORE_MODEL_H

#include <string>

namespace lumagl {

class Model {
 public:
  class Options;

  explicit Model(void *device, const Options &);
  explicit Model(void *device);
  void draw();

  // wgpu::ShaderModule vsModule;  // Compiled vertex shader
  // wgpu::ShaderModule fsModule;  // Compiled fragment shader
  // wgpu::RenderPipeline pipeline;  // rendering pipeline ("program")
  // wgpu::BindGroupLayout bgl;  // Uniform buffer
};

class Model::Options {
 public:
  std::string vs;  // vertex shader source
  std::string fs;  // fragment shader source
};

}  // namespace lumagl

#endif  // LUMAGL_CORE_MODEL_H
