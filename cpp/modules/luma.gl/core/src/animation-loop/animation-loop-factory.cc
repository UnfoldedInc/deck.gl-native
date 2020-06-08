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

#include "./animation-loop-factory.h"  // NOLINT(build/include)

using namespace lumagl;

auto AnimationLoopFactory::createAnimationLoop(const std::shared_ptr<AnimationLoop::Options>& options,
                                               probegl::Error& error) noexcept -> std::shared_ptr<AnimationLoop> {
  return probegl::catchError<AnimationLoop>([options]() { return createAnimationLoop(options); }, error);
}

auto AnimationLoopFactory::createAnimationLoop(probegl::Error& error) noexcept -> std::shared_ptr<AnimationLoop> {
  return probegl::catchError<AnimationLoop>([]() { return createAnimationLoop(); }, error);
}

auto AnimationLoopFactory::createAnimationLoop(const std::shared_ptr<AnimationLoop::Options>& options)
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

#if defined(LUMAGL_USES_GLFW)
  if (auto glfwOptions = std::dynamic_pointer_cast<GLFWAnimationLoop::Options>(options)) {
    return std::make_shared<GLFWAnimationLoop>(*glfwOptions.get());
  }
#endif

#if defined(LUMAGL_ENABLE_BACKEND_METAL)
  if (auto metalOptions = std::dynamic_pointer_cast<MetalAnimationLoop::Options>(options)) {
    return std::make_shared<MetalAnimationLoop>(*metalOptions.get());
  }
#endif

  return std::make_shared<AnimationLoop>(*options.get());
}
