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
  this->mStartTime = mach_absolute_time();
  // Cache secondCoeff
  this->_getSecondCoeff();
  this->mRunning = true;
}

void Timer::stop() {
  this->mStopTime = mach_absolute_time();
  this->mRunning = false;
}

auto Timer::getElapsedTime() const -> double {
  if (this->mRunning) {
    return this->mSecondCoeff * (mach_absolute_time() - this->mStartTime);
  } else {
    return this->mSecondCoeff * (this->mStopTime - this->mStartTime);
  }
}

auto Timer::getAbsoluteTime() -> double { return this->_getSecondCoeff() * mach_absolute_time(); }

auto Timer::_getSecondCoeff() -> double {
  // If this is the first time we've run, get the timebase.
  if (this->mSecondCoeff == 0.0) {
    mach_timebase_info_data_t timebaseInfo;
    mach_timebase_info(&timebaseInfo);
    this->mSecondCoeff = timebaseInfo.numer * (1.0 / 1000000000) / timebaseInfo.denom;
  }

  return this->mSecondCoeff;
}

#elif defined(PROBEGL_PLATFORM_POSIX)

static uint64_t GetCurrentTimeNs() {
  struct timespec currentTime;
  clock_gettime(CLOCK_MONOTONIC, &currentTime);
  return currentTime.tv_sec * 1'000'000'000llu + currentTime.tv_nsec;
}

void Timer::Start() {
  mStartTimeNs = GetCurrentTimeNs();
  mRunning = true;
}

void Timer::Stop() {
  mStopTimeNs = GetCurrentTimeNs();
  mRunning = false;
}

double Timer::GetElapsedTime() const {
  uint64_t endTimeNs;
  if (mRunning) {
    endTimeNs = GetCurrentTimeNs();
  } else {
    endTimeNs = mStopTimeNs;
  }

  return (endTimeNs - mStartTimeNs) * 1e-9;
}

double Timer::GetAbsoluteTime() { return GetCurrentTimeNs() * 1e-9; }

#elif defined(PROBEGL_PLATFORM_WINDOWS)

void Timer::Start() {
  LARGE_INTEGER curTime;
  QueryPerformanceCounter(&curTime);
  mStartTime = curTime.QuadPart;

  // Cache the frequency
  GetFrequency();

  mRunning = true;
}

void Timer::Stop() {
  LARGE_INTEGER curTime;
  QueryPerformanceCounter(&curTime);
  mStopTime = curTime.QuadPart;

  mRunning = false;
}

double Timer::GetElapsedTime() const {
  LONGLONG endTime;
  if (mRunning) {
    LARGE_INTEGER curTime;
    QueryPerformanceCounter(&curTime);
    endTime = curTime.QuadPart;
  } else {
    endTime = mStopTime;
  }

  return static_cast<double>(endTime - mStartTime) / mFrequency;
}

double Timer::GetAbsoluteTime() {
  LARGE_INTEGER curTime;
  QueryPerformanceCounter(&curTime);

  return static_cast<double>(curTime.QuadPart) / GetFrequency();
}

LONGLONG Timer::GetFrequency() {
  if (mFrequency == 0) {
    LARGE_INTEGER frequency = {};
    QueryPerformanceFrequency(&frequency);

    mFrequency = frequency.QuadPart;
  }

  return mFrequency;
}

#endif
