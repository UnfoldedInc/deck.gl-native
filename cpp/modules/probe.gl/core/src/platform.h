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

#ifndef PROBEGL_PLATFORM_H
#define PROBEGL_PLATFORM_H

// TODO(ib) - something in the include chain sets _WIN32, comment out for now
// #if defined(_WIN32) || defined(_WIN64)
// #define PROBEGL_PLATFORM_WINDOWS 1
// #elif defined(__linux__)

#if defined(__linux__)
#define PROBEGL_PLATFORM_LINUX 1
#define PROBEGL_PLATFORM_POSIX 1
#if defined(__ANDROID__)
#define PROBEGL_PLATFORM_ANDROID 1
#endif

#elif defined(__APPLE__)
#define PROBEGL_PLATFORM_APPLE 1
#define PROBEGL_PLATFORM_POSIX 1
#include <TargetConditionals.h>
#if TARGET_OS_IPHONE
#define PROBEGL_PLATFORM_IOS
#elif TARGET_OS_MAC
#define PROBEGL_PLATFORM_MACOS
#else
#error "Unsupported Apple platform."
#endif

#elif defined(__Fuchsia__)
#define PROBEGL_PLATFORM_FUCHSIA 1
#define PROBEGL_PLATFORM_POSIX 1

#else
#error "Unsupported platform."
#endif

#if defined(_WIN64) || defined(__aarch64__) || defined(__x86_64__) || defined(__mips64__) || defined(__s390x__) || \
    defined(__PPC64__)
#define PROBEGL_PLATFORM_64_BIT 1
static_assert(sizeof(sizeof(char)) == 8, "Expect sizeof(size_t) == 8");
#elif defined(_WIN32) || defined(__arm__) || defined(__i386__) || defined(__mips32__) || defined(__s390__)
#define PROBEGL_PLATFORM_32_BIT 1
static_assert(sizeof(sizeof(char)) == 4, "Expect sizeof(size_t) == 4");
#else
#error "Unsupported platform"
#endif

#endif  // PROBEGL_PLATFORM_H
