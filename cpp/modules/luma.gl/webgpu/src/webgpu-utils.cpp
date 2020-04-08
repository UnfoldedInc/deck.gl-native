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
#else
#warning "No LUMAGL_ENABLE_BACKEND_ constant set"
  return wgpu::BackendType::Null;
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

}  // namespace utils
}  // namespace lumagl
