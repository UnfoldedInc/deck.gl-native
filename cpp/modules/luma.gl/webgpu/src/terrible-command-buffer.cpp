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

#include "./terrible-command-buffer.h"  // NOLINT(build/include)

#include "probe.gl/core.h"

using namespace lumagl;
using namespace lumagl::utils;

TerribleCommandBuffer::TerribleCommandBuffer() {}

TerribleCommandBuffer::TerribleCommandBuffer(dawn_wire::CommandHandler* handler) : mHandler(handler) {}

void TerribleCommandBuffer::SetHandler(dawn_wire::CommandHandler* handler) { mHandler = handler; }

void* TerribleCommandBuffer::GetCmdSpace(size_t size) {
  // TODO(kainino@chromium.org): Should we early-out if size is 0?
  //   (Here and/or in the caller?) It might be good to make the wire receiver get a nullptr
  //   instead of pointer to zero-sized allocation in mBuffer.

  // Cannot have commands in mBuffer and mLargeBuffer at same time.
  ASSERT(mOffset == 0 || mLargeBufferCmdSize == 0);

  if (size > sizeof(mBuffer)) {
    // Flush current cmds in mBuffer to keep order.
    if (mOffset > 0) {
      if (!Flush()) {
        return nullptr;
      }
      return GetCmdSpace(size);
    }

    // Resize large buffer to the size that can
    // contain incoming command if needed.
    if (mLargeBuffer.size() < size) {
      mLargeBuffer.resize(size);
    }

    // Record whole cmd space.
    mLargeBufferCmdSize = size;

    return mLargeBuffer.data();
  }

  // Trigger flush if large buffer contain cmds.
  if (mLargeBufferCmdSize > 0) {
    if (!Flush()) {
      return nullptr;
    }
    return GetCmdSpace(size);
  }

  // Need to flush large buffer first.
  ASSERT(mLargeBufferCmdSize == 0);

  char* result = &mBuffer[mOffset];

  if (sizeof(mBuffer) - size < mOffset) {
    if (!Flush()) {
      return nullptr;
    }
    return GetCmdSpace(size);
  }

  mOffset += size;

  return result;
}

bool TerribleCommandBuffer::Flush() {
  // Cannot have commands in mBuffer and mLargeBuffer at same time.
  ASSERT(mOffset == 0 || mLargeBufferCmdSize == 0);

  bool success = false;
  // Big buffer not empty, flush it!
  if (mLargeBufferCmdSize > 0) {
    success = mHandler->HandleCommands(mLargeBuffer.data(), mLargeBufferCmdSize) != nullptr;
    // Clear big command buffers.
    mLargeBufferCmdSize = 0;
    return success;
  }

  success = mHandler->HandleCommands(mBuffer, mOffset) != nullptr;
  mOffset = 0;

  return success;
}
