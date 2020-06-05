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

#ifndef PROBEGL_CORE_COMPILER_H
#define PROBEGL_CORE_COMPILER_H

// Input Macros
//  - PROBEGL_CPP_VERSION

// Defines macros for compiler-specific functionality
//  - PROBEGL_COMPILER_[CLANG|GCC|MSVC]: Compiler detection
//  - PROBEGL_BREAKPOINT(): Raises an exception and breaks in the debugger
//  - PROBEGL_BUILTIN_UNREACHABLE(): Hints the compiler that a code path is unreachable
//  - PROBEGL_NO_DISCARD: An attribute that is C++17 [[nodiscard]] where available
//  - PROBEGL_(UN)?LIKELY(EXPR): Where available, hints the compiler that the expression will be true
//      (resp. false) to help it generate code that leads to better branch prediction.
//  - PROBEGL_UNUSED(EXPR): Prevents unused variable/expression warnings on EXPR.
//  - PROBEGL_UNUSED_FUNC(FUNC): Prevents unused function warnings on FUNC.
//  - PROBEGL_DECLARE_UNUSED:    Prevents unused function warnings a subsequent declaration.
//  Both PROBEGL_UNUSED_FUNC and PROBEGL_DECLARE_UNUSED may be necessary, e.g. to suppress clang's
//  unneeded-internal-declaration warning.

// Clang and GCC, check for __clang__ too to catch clang-cl masquarading as MSVC
#if defined(__GNUC__) || defined(__clang__)
#if defined(__clang__)
#define PROBEGL_COMPILER_CLANG
#else
#define PROBEGL_COMPILER_GCC
#endif

#if defined(__i386__) || defined(__x86_64__)
#define PROBEGL_BREAKPOINT() __asm__ __volatile__("int $3\n\t")
#else
#define PROBEGL_BREAKPOINT()
#endif

#define PROBEGL_BUILTIN_UNREACHABLE() __builtin_unreachable()
#define PROBEGL_LIKELY(x) __builtin_expect(!!(x), 1)
#define PROBEGL_UNLIKELY(x) __builtin_expect(!!(x), 0)

#if !defined(__has_cpp_attribute)
#define __has_cpp_attribute(name) 0
#endif

// Use warn_unused_result on clang otherwise we can a c++1z extension warning in C++14 mode
// Also avoid warn_unused_result with GCC because it is only a function attribute and not a type
// attribute.
#if __has_cpp_attribute(warn_unused_result) && defined(__clang__)
#define PROBEGL_NO_DISCARD __attribute__((warn_unused_result))
#elif PROBEGL_CPP_VERSION >= 17 && __has_cpp_attribute(nodiscard)
#define PROBEGL_NO_DISCARD [[nodiscard]]  // NOLINT
#endif

#define PROBEGL_DECLARE_UNUSED __attribute__((unused))
#if defined(NDEBUG)
#define PROBEGL_FORCE_INLINE inline __attribute__((always_inline))
#endif

// MSVC
#elif defined(_MSC_VER)
#define PROBEGL_COMPILER_MSVC

extern void __cdecl __debugbreak(void);
#define PROBEGL_BREAKPOINT() __debugbreak()

#define PROBEGL_BUILTIN_UNREACHABLE() __assume(false)

// Visual Studio 2017 15.3 adds support for [[nodiscard]]
#if _MSC_VER >= 1911 && PROBEGL_CPP_VERSION >= 17
#define PROBEGL_NO_DISCARD [[nodiscard]]  // NOLINT
#endif

#define PROBEGL_DECLARE_UNUSED
#if defined(NDEBUG)
#define PROBEGL_FORCE_INLINE __forceinline
#endif

#else
#error "Unsupported compiler"
#endif

// It seems that (void) EXPR works on all compilers to silence the unused variable warning.
#define PROBEGL_UNUSED(EXPR) (void)EXPR
// Likewise using static asserting on sizeof(&FUNC) seems to make it tagged as used
#define PROBEGL_UNUSED_FUNC(FUNC) static_assert(sizeof(&FUNC) == sizeof(void (*)()), "")

// Add noop replacements for macros for features that aren't supported by the compiler.
#if !defined(PROBEGL_LIKELY)
#define PROBEGL_LIKELY(X) X
#endif
#if !defined(PROBEGL_UNLIKELY)
#define PROBEGL_UNLIKELY(X) X
#endif
#if !defined(PROBEGL_NO_DISCARD)
#define PROBEGL_NO_DISCARD
#endif
#if !defined(PROBEGL_FORCE_INLINE)
#define PROBEGL_FORCE_INLINE inline
#endif

#endif  // PROBEGL_CORE_COMPILER_H
