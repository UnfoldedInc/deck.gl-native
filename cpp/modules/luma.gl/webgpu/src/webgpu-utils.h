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

#ifndef LUMAGL_UTILS_WEBGPU_UTILS_H
#define LUMAGL_UTILS_WEBGPU_UTILS_H

#include <dawn/webgpu_cpp.h>

#include <string>

namespace lumagl {
namespace utils {

/// \brief Get the default WebGPU/Dawn backend type for this platform
/// Default to D3D12, Metal, Vulkan, OpenGL in that order as D3D12 and Metal are the preferred on
/// their respective platforms, and Vulkan is preferred to OpenGL
auto getDefaultWebGPUBackendType() -> wgpu::BackendType;

/// \brief Get a WebGPU/Dawn backend type from a string
auto getWebGPUBackendType(const std::string &backendType) -> wgpu::BackendType;

/// \brief Get a string representation of a WebGPU Error
auto getDeviceErrorTypeName(WGPUErrorType errorType) -> const char *;

}  // namespace utils
}  // namespace lumagl

#endif  // LUMAGL_UTILS_WEBGPU_UTILS_H
