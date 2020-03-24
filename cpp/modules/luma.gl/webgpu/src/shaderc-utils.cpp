#include "./shaderc-utils.h"

#include <shaderc/shaderc.hpp>

auto ShadercShaderKind(SingleShaderStage stage) -> shaderc_shader_kind {
  switch (stage) {
    case SingleShaderStage::Vertex:
      return shaderc_glsl_vertex_shader;
    case SingleShaderStage::Fragment:
      return shaderc_glsl_fragment_shader;
    case SingleShaderStage::Compute:
      return shaderc_glsl_compute_shader;
    default:
      UNREACHABLE();
  }
}

auto CreateShaderModuleFromResult(const wgpu::Device& device,
                                                const shaderc::SpvCompilationResult& result) -> wgpu::ShaderModule {
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

}  // anonymous namespace

auto CreateShaderModule(const wgpu::Device& device, SingleShaderStage stage, const char* source) -> wgpu::ShaderModule {
  shaderc_shader_kind kind = ShadercShaderKind(stage);

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

  return CreateShaderModuleFromResult(device, result);
}

auto CreateShaderModuleFromASM(const wgpu::Device& device, const char* source) -> wgpu::ShaderModule {
  shaderc::Compiler compiler;
  shaderc::SpvCompilationResult result = compiler.AssembleToSpv(source, strlen(source));
  if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
    dawn::ErrorLog() << result.GetErrorMessage();
    return {};
  }

  return CreateShaderModuleFromResult(device, result);
}
