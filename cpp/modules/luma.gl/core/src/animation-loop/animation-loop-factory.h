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

#ifndef LUMAGL_CORE_ANIMATION_LOOP_FACTORY_H
#define LUMAGL_CORE_ANIMATION_LOOP_FACTORY_H

#include <memory>
#include <optional>

#include "luma.gl/core.h"
#include "probe.gl/core.h"

namespace lumagl {

// TODO(ilija@unfolded.ai): Revisit this API
struct AnimationLoopFactory {
 public:
#pragma mark - Exception-free API

  static auto createAnimationLoop(const std::shared_ptr<AnimationLoop::Options>& options,
                                  probegl::Error& error) noexcept -> std::shared_ptr<AnimationLoop>;
  static auto createAnimationLoop(probegl::Error& error) noexcept -> std::shared_ptr<AnimationLoop>;

#pragma mark -

  static auto createAnimationLoop(const std::shared_ptr<AnimationLoop::Options>& options = nullptr)
      -> std::shared_ptr<AnimationLoop>;
};

}  // namespace lumagl

#endif  // LUMAGL_CORE_ANIMATION_LOOP_FACTORY_H
