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

#include "./shaderc-utils.h"  // NOLINT(build/include)

#include "probe.gl/core.h"

// #include <shaderc/shaderc.hpp>
enum shaderc_shader_kind { shaderc_glsl_vertex_shader, shaderc_glsl_fragment_shader, shaderc_glsl_compute_shader };

using namespace lumagl;
using namespace lumagl::utils;

namespace {

auto shadercShaderKind(SingleShaderStage stage) -> shaderc_shader_kind {
  switch (stage) {
    case SingleShaderStage::Vertex:
      return shaderc_glsl_vertex_shader;
    case SingleShaderStage::Fragment:
      return shaderc_glsl_fragment_shader;
    case SingleShaderStage::Compute:
      return shaderc_glsl_compute_shader;
    default:
      PROBEGL_UNREACHABLE();
  }
}

/*
auto createShaderModuleFromResult(const wgpu::Device& device, const shaderc::SpvCompilationResult& result)
    -> wgpu::ShaderModule {
  // result.cend and result.cbegin return pointers to uint32_t.
  const uint32_t* resultBegin = result.cbegin();
  const uint32_t* resultEnd = result.cend();
  // So this size is in units of sizeof(uint32_t).
  ptrdiff_t resultSize = resultEnd - resultBegin;
  // SetSource takes data as uint32_t*.

  wgpu::ShaderModuleDescriptor descriptor;
  descriptor.codeSize = static_cast<uint32_t>(resultSize);
  descriptor.code = result.cbegin();
  return device.CreateShaderModule(&descriptor);
}
*/

}  // anonymous namespace

auto createShaderModule(const wgpu::Device& device, SingleShaderStage stage, const char* source) -> wgpu::ShaderModule {
  /*
  shaderc_shader_kind kind = shadercShaderKind(stage);

  shaderc::Compiler compiler;
  auto result = compiler.CompileGlslToSpv(source, strlen(source), kind, "myshader?");
  if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
    dawn::ErrorLog() << result.GetErrorMessage();
    return {};
  }
#ifdef DUMP_SPIRV_ASSEMBLY
  {
    shaderc::CompileOptions options;
    auto resultAsm = compiler.CompileGlslToSpvAssembly(source, strlen(source), kind, "myshader?", options);
    size_t sizeAsm = (resultAsm.cend() - resultAsm.cbegin());

    char* buffer = reinterpret_cast<char*>(malloc(sizeAsm + 1));
    memcpy(buffer, resultAsm.cbegin(), sizeAsm);
    buffer[sizeAsm] = '\0';
    printf("SPIRV ASSEMBLY DUMP START\n%s\nSPIRV ASSEMBLY DUMP END\n", buffer);
    free(buffer);
  }
#endif

#ifdef DUMP_SPIRV_JS_ARRAY
  printf("SPIRV JS ARRAY DUMP START\n");
  for (size_t i = 0; i < size; i++) {
    printf("%#010x", result.cbegin()[i]);
    if ((i + 1) % 4 == 0) {
      printf(",\n");
    } else {
      printf(", ");
    }
  }
  printf("\n");
  printf("SPIRV JS ARRAY DUMP END\n");
#endif

  return createShaderModuleFromResult(device, result);
  */
  return nullptr;
}

auto createShaderModuleFromASM(const wgpu::Device& device, const char* source) -> wgpu::ShaderModule {
  /*
  shaderc::Compiler compiler;
  shaderc::SpvCompilationResult result = compiler.AssembleToSpv(source, strlen(source));
  if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
    dawn::ErrorLog() << result.GetErrorMessage();
    return {};
  }

  return CreateShaderModuleFromResult(device, result);
  */
  return nullptr;
}
