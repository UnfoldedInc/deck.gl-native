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

/// \file This file contains utilities for the WebGPU C++ API

#include "./webgpu-utils.h"  // NOLINT(build/include)

#include <map>

namespace lumagl {
namespace utils {

auto getDefaultWebGPUBackendType() -> wgpu::BackendType {
#if defined(LUMAGL_ENABLE_BACKEND_D3D12)
  return wgpu::BackendType::D3D12;
#elif defined(LUMAGL_ENABLE_BACKEND_METAL)
  return wgpu::BackendType::Metal;
#elif defined(LUMAGL_ENABLE_BACKEND_VULKAN)
  return wgpu::BackendType::Vulkan;
#elif defined(LUMAGL_ENABLE_BACKEND_OPENGL)
  return wgpu::BackendType::OpenGL;
#elif defined(LUMAGL_ENABLE_BACKEND_NULL)
  return wgpu::BackendType::Null;
#else
#error No backends enabled
#endif
}

const std::map<const std::string, wgpu::BackendType> nameToBackendTypeMap = {
    {"d3d12", wgpu::BackendType::D3D12},   {"metal", wgpu::BackendType::Metal},   {"null", wgpu::BackendType::Null},
    {"opengl", wgpu::BackendType::OpenGL}, {"vulkan", wgpu::BackendType::Vulkan},
};

auto getWebGPUBackendType(const std::string &backendName) -> wgpu::BackendType {
  if (nameToBackendTypeMap.count(backendName) == 0) {
    throw std::runtime_error("invalid backend name (opengl, metal, d3d12, null, vulkan)");
  }
  return nameToBackendTypeMap.at(backendName);
}

std::map<WGPUErrorType, const char *> errorTypeToNameMap = {
    {WGPUErrorType_Validation, "Validation"},
    {WGPUErrorType_OutOfMemory, "Out of memory"},
    {WGPUErrorType_Unknown, "Unknown"},
    {WGPUErrorType_DeviceLost, "Device lost"},
};

auto getWebGPUErrorName(WGPUErrorType errorType) -> const char * { return errorTypeToNameMap[errorType]; }

auto getVertexFormatSize(wgpu::VertexFormat format) -> size_t {
  // Based on https://gpuweb.github.io/gpuweb/#vertex-formats
  // uchar = unsigned 8-bit value
  // char = signed 8-bit value
  // ushort = unsigned 16-bit value
  // short = signed 16-bit value
  // half = half-precision 16-bit floating point value
  // float = 32-bit floating point value
  // uint = unsigned 32-bit integer value
  // int = signed 32-bit integer value

  using Format = wgpu::VertexFormat;
  switch (format) {
    case Format::UChar2:
    case Format::Char2:
    case Format::UChar2Norm:
    case Format::Char2Norm:
      return 2;
    case Format::UChar4:
    case Format::Char4:
    case Format::UChar4Norm:
    case Format::Char4Norm:
    case Format::UShort2:
    case Format::Short2:
    case Format::UShort2Norm:
    case Format::Short2Norm:
    case Format::Half2:
    case Format::Float:
    case Format::UInt:
    case Format::Int:
      return 4;
    case Format::UShort4:
    case Format::Short4:
    case Format::UShort4Norm:
    case Format::Short4Norm:
    case Format::Half4:
    case Format::Float2:
    case Format::UInt2:
    case Format::Int2:
      return 8;
    case Format::Float3:
    case Format::UInt3:
    case Format::Int3:
      return 12;
    case Format::Float4:
    case Format::UInt4:
    case Format::Int4:
      return 16;
  }
}

}  // namespace utils
}  // namespace lumagl
