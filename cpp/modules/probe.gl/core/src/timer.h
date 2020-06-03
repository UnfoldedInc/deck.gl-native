// Copyright 2019 The Dawn Authors
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

#ifndef PROBEGL_CORE_TIMER_H
#define PROBEGL_CORE_TIMER_H

#include "./platform.h"

#if defined(PROBEGL_PLATFORM_APPLE)

#include <CoreServices/CoreServices.h>
#include <mach/mach.h>
#include <mach/mach_time.h>

#elif defined(PROBEGL_PLATFORM_POSIX)

#include <stdint.h>
#include <time.h>

#elif defined(PROBEGL_PLATFORM_WINDOWS)

#include <windows.h>

#else

#warning "No timer implementation for current platform"

#endif

namespace probegl {

class Timer {
 public:
  virtual ~Timer() {}

  // Timer functionality: Use start() and stop() to record the duration and use
  // getElapsedTime() to query that duration.  If getElapsedTime() is called in between, it
  // will report the elapsed time since start().
  void start();
  void stop();
  double getElapsedTime() const;

  // Timestamp functionality: Use getAbsoluteTime() to get an absolute time with an unknown
  // origin. This time moves forward regardless of start()/stop().
  double getAbsoluteTime();

 private:
  bool running{false};
#if defined(PROBEGL_PLATFORM_APPLE)
  // APPLE
  uint64_t startTime;
  uint64_t stopTime;
  double secondCoeff{0};
  auto _getSecondCoeff() -> double;
#elif defined(PROBEGL_PLATFORM_POSIX)
  // POSIX
  uint64_t startTimeNs;
  uint64_t stopTimeNs;
#elif defined(PROBEGL_PLATFORM_WINDOWS)
  // WINDOWS
  LONGLONG startTime;
  LONGLONG stopTime;
  LONGLONG frequency{0};
#endif
};

}  // namespace probegl

#endif  // PROBEGL_CORE_TIMER_H
