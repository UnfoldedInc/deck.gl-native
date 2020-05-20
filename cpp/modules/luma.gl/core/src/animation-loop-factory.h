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

namespace lumagl {

struct AnimationLoopFactory {
 public:
  // TODO(ilija@unfolded.ai): Revisit
  static auto createAnimationLoop(const std::optional<AnimationLoop::Options*>& options)
      -> std::shared_ptr<AnimationLoop> {
    // If no options are passed, try and initialize the only setup which requires no arguments, which is GLFW
    if (!options) {
#if defined(LUMAGL_USES_GLFW)
      GLFWAnimationLoop::Options opts;
      return std::make_shared<GLFWAnimationLoop>(opts);
#else
      return nullptr;
#endif
    }

    auto opts = options.value();
#if defined(LUMAGL_USES_GLFW)
    if (auto glfwOptions = dynamic_cast<GLFWAnimationLoop::Options*>(opts)) {
      return std::make_shared<GLFWAnimationLoop>(*glfwOptions);
    }
#endif

#if defined(LUMAGL_ENABLE_BACKEND_METAL)
    if (auto metalOptions = dynamic_cast<MetalAnimationLoop::Options*>(opts)) {
      return std::make_shared<MetalAnimationLoop>(*metalOptions);
    }
#endif

    return std::make_shared<AnimationLoop>(*opts);
  }
};

}  // namespace lumagl

#endif  // LUMAGL_CORE_ANIMATION_LOOP_FACTORY_H
