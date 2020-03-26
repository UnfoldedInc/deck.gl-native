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

#ifndef LUMAGL_WEBGPU_CONSTANTS_H
#define LUMAGL_WEBGPU_CONSTANTS_H

#include <cstdint>

namespace lumagl {
namespace utils {

static constexpr uint32_t kMaxBindGroups = 4u;
// TODO(cwallez@chromium.org): investigate bindgroup limits
static constexpr uint32_t kMaxBindingsPerGroup = 16u;
static constexpr uint32_t kMaxVertexAttributes = 16u;
// Vulkan has a standalone limit named maxVertexInputAttributeOffset (2047u at least) for vertex
// attribute offset. The limit might be meaningless because Vulkan has another limit named
// maxVertexInputBindingStride (2048u at least). We use maxVertexAttributeEnd (2048u) here to
// verify vertex attribute offset, which equals to maxOffset + smallest size of vertex format
// (char). We may use maxVertexInputBindingStride (maxVertexBufferStride below) instead to replace
// maxVertexAttributeEnd in future.
static constexpr uint32_t kMaxVertexAttributeEnd = 2048u;
static constexpr uint32_t kMaxVertexBuffers = 16u;
static constexpr uint32_t kMaxVertexBufferStride = 2048u;
static constexpr uint32_t kNumStages = 3;
static constexpr uint32_t kMaxColorAttachments = 4u;
static constexpr uint32_t kTextureRowPitchAlignment = 256u;
// Dynamic buffer offsets require offset to be divisible by 256
static constexpr uint64_t kMinDynamicBufferOffsetAlignment = 256u;
// Max numbers of dynamic uniform buffers
static constexpr uint32_t kMaxDynamicUniformBufferCount = 8u;
// Max numbers of dynamic storage buffers
static constexpr uint32_t kMaxDynamicStorageBufferCount = 4u;
// Max numbers of dynamic buffers
static constexpr uint32_t kMaxDynamicBufferCount = kMaxDynamicUniformBufferCount + kMaxDynamicStorageBufferCount;
// Indirect command sizes
static constexpr uint64_t kDispatchIndirectSize = 3 * sizeof(uint32_t);
static constexpr uint64_t kDrawIndirectSize = 4 * sizeof(uint32_t);
static constexpr uint64_t kDrawIndexedIndirectSize = 5 * sizeof(uint32_t);

// Non spec defined constants.
static constexpr float kLodMin = 0.0;
static constexpr float kLodMax = 1000.0;

// Max texture size constants
static constexpr uint32_t kMaxTextureSize = 8192u;
static constexpr uint32_t kMaxTexture2DArrayLayers = 256u;
static constexpr uint32_t kMaxTexture2DMipLevels = 14u;
static_assert(1 << (kMaxTexture2DMipLevels - 1) == kMaxTextureSize,
              "kMaxTexture2DMipLevels and kMaxTextureSize size mismatch");

}  // namespace utils
}  // namespace lumagl

#endif  // LUMAGL_WEBGPU_CONSTANTS_H
