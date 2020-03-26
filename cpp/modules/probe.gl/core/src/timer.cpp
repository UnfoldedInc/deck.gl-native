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

#include "./timer.h"

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
