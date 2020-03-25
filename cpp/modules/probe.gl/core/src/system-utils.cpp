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

#include "./system-utils.h"  // NOLINT(build/include)

#include "./platform.h"

#if defined(PROBEGL_PLATFORM_WINDOWS)
#include <Windows.h>
#elif defined(PROBEGL_PLATFORM_POSIX)
#include <unistd.h>
#else
#error "Unsupported platform."
#endif

using namespace probegl;

#if defined(PROBGL_PLATFORM_WINDOWS)

void uSleep(unsigned int usecs) { Sleep(static_cast<DWORD>(usecs / 1000)); }

#elif defined(PROBEGL_PLATFORM_POSIX)

void uSleep(unsigned int usecs) { usleep(usecs); }

#else
#error "Implement uSleep for your platform."
#endif
