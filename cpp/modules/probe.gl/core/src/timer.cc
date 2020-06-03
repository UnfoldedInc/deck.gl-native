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

#include "./timer.h"  // NOLINT(build/include)

using namespace probegl;

#if defined(PROBEGL_PLATFORM_APPLE)

void Timer::start() {
  this->startTime = mach_absolute_time();
  // Cache secondCoeff
  this->_getSecondCoeff();
  this->running = true;
}

void Timer::stop() {
  this->stopTime = mach_absolute_time();
  this->running = false;
}

auto Timer::getElapsedTime() const -> double {
  if (this->running) {
    return this->secondCoeff * (mach_absolute_time() - this->startTime);
  } else {
    return this->secondCoeff * (this->stopTime - this->startTime);
  }
}

auto Timer::getAbsoluteTime() -> double { return this->_getSecondCoeff() * mach_absolute_time(); }

auto Timer::_getSecondCoeff() -> double {
  // If this is the first time we've run, get the timebase.
  if (this->secondCoeff == 0.0) {
    mach_timebase_info_data_t timebaseInfo;
    mach_timebase_info(&timebaseInfo);
    this->secondCoeff = timebaseInfo.numer * (1.0 / 1000000000) / timebaseInfo.denom;
  }

  return this->secondCoeff;
}

#elif defined(PROBEGL_PLATFORM_POSIX)

static uint64_t getCurrentTimeNs() {
  struct timespec currentTime;
  clock_gettime(CLOCK_MONOTONIC, &currentTime);
  return currentTime.tv_sec * 1'000'000'000llu + currentTime.tv_nsec;
}

void Timer::start() {
  this->startTimeNs = getCurrentTimeNs();
  this->running = true;
}

void Timer::stop() {
  this->stopTimeNs = getCurrentTimeNs();
  this->running = false;
}

double Timer::getElapsedTime() const {
  uint64_t endTimeNs;
  if (this->running) {
    endTimeNs = getCurrentTimeNs();
  } else {
    endTimeNs = this->stopTimeNs;
  }

  return (endTimeNs - this->startTimeNs) * 1e-9;
}

double Timer::getAbsoluteTime() { return getCurrentTimeNs() * 1e-9; }

#elif defined(PROBEGL_PLATFORM_WINDOWS)

void Timer::start() {
  LARGE_INTEGER curTime;
  QueryPerformanceCounter(&curTime);
  this->startTime = curTime.QuadPart;

  // Cache the frequency
  this->getFrequency();

  this->running = true;
}

void Timer::stop() {
  LARGE_INTEGER curTime;
  QueryPerformanceCounter(&curTime);
  this->stopTime = curTime.QuadPart;

  this->running = false;
}

double Timer::getElapsedTime() const {
  LONGLONG endTime;
  if (running) {
    LARGE_INTEGER curTime;
    QueryPerformanceCounter(&curTime);
    endTime = curTime.QuadPart;
  } else {
    endTime = this->stopTime;
  }

  return static_cast<double>(endTime - this->startTime) / this->frequency;
}

double Timer::getAbsoluteTime() {
  LARGE_INTEGER curTime;
  QueryPerformanceCounter(&curTime);

  return static_cast<double>(curTime.QuadPart) / this->getFrequency();
}

LONGLONG Timer::getFrequency() {
  if (this->frequency == 0) {
    LARGE_INTEGER frequency = {};
    QueryPerformanceFrequency(&frequency);

    this->frequency = frequency.QuadPart;
  }

  return this->frequency;
}

#endif
