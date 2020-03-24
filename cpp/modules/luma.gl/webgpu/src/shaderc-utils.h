// Copyright 2017 The Dawn Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef LUMAGL_UTILS_SHADERC_UTILS_H
#define LUMAGL_UTILS_SHADERC_UTILS_H

#include <dawn/webgpu_cpp.h>

#include <array>
#include <initializer_list>

#include "./webgpu-constants.h"

namespace lumagl {
namespace utils {

enum Expectation { Success, Failure };

enum class SingleShaderStage { Vertex, Fragment, Compute };

wgpu::ShaderModule CreateShaderModule(const wgpu::Device& device, SingleShaderStage stage, const char* source);
wgpu::ShaderModule CreateShaderModuleFromASM(const wgpu::Device& device, const char* source);

}  // namespace utils
}  // namespace lumagl

#endif // LUMAGL_UTILS_SHADERC_UTILS_H
