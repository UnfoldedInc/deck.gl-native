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

#ifndef COMMON_ASSERT_H_
#define COMMON_ASSERT_H_

#include "./compiler.h"

// Asserts to be used instead of the regular C stdlib assert function.
// In debug ASSERT(condition) will trigger an error, otherwise in
// release it does nothing at runtime.
//
// In case of name clashes (with for example a testing library), you can define the
// PROBEGL_SKIP_ASSERT_SHORTHANDS to only define the PROBEGL_ prefixed macros.
//
// These asserts feature:
//     - Logging of the error with file, line and function information.
//     - Breaking in the debugger when an assert is triggered and a debugger is attached.
//     - Use the assert information to help the compiler optimizer in release builds.

// MSVC triggers a warning in /W4 for do {} while(0). SDL worked around this by using (0,0) and
// points out that it looks like an owl face.
#if defined(PROBEGL_COMPILER_MSVC)
#define PROBEGL_ASSERT_LOOP_CONDITION (0, 0)
#else
#define PROBEGL_ASSERT_LOOP_CONDITION (0)
#endif

// PROBEGL_ASSERT_CALLSITE_HELPER generates the actual assert code. In Debug it does what you would
// expect of an assert and in release it tries to give hints to make the compiler generate better
// code.
#if defined(PROBEGL_ENABLE_ASSERTS)
#define PROBEGL_ASSERT_CALLSITE_HELPER(file, func, line, condition) \
  do {                                                              \
    if (!(condition)) {                                             \
      HandleAssertionFailure(file, func, line, #condition);         \
    }                                                               \
  } while (PROBEGL_ASSERT_LOOP_CONDITION)
#else
#if defined(PROBEGL_COMPILER_MSVC)
#define PROBEGL_ASSERT_CALLSITE_HELPER(file, func, line, condition) __assume(condition)
#elif defined(PROBEGL_COMPILER_CLANG) && defined(__builtin_assume)
#define PROBEGL_ASSERT_CALLSITE_HELPER(file, func, line, condition) __builtin_assume(condition)
#else
#define PROBEGL_ASSERT_CALLSITE_HELPER(file, func, line, condition) \
  do {                                                              \
    PROBEGL_UNUSED(sizeof(condition));                              \
  } while (PROBEGL_ASSERT_LOOP_CONDITION)
#endif
#endif

#define PROBEGL_ASSERT(condition) PROBEGL_ASSERT_CALLSITE_HELPER(__FILE__, __func__, __LINE__, condition)
#define PROBEGL_UNREACHABLE()                                                \
  do {                                                                       \
    PROBEGL_ASSERT(PROBEGL_ASSERT_LOOP_CONDITION && "Unreachable code hit"); \
    PROBEGL_BUILTIN_UNREACHABLE();                                           \
  } while (PROBEGL_ASSERT_LOOP_CONDITION)

#if !defined(PROBEGL_SKIP_ASSERT_SHORTHANDS)
#define ASSERT PROBEGL_ASSERT
#define UNREACHABLE PROBEGL_UNREACHABLE
#endif

void HandleAssertionFailure(const char* file, const char* function, int line, const char* condition);

#endif  // COMMON_ASSERT_H_
