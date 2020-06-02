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

#ifndef PROBEGL_LOG_H_
#define PROBEGL_LOG_H_

// Don't use iostream or printf directly for several reasons:
//  - iostream adds static initializers which we want to avoid.
//  - printf and iostream don't show up in logcat on Android so printf debugging doesn't work but
//  log-message debugging does.
//  - log severity helps provide intent compared to a printf.
//
// Logging should in general be avoided: errors should go through the regular WebGPU error reporting
// mechanism and others form of logging should (TODO: eventually) go through the logging dependency
// injection, so for example they show up in Chromium's about:gpu page. Nonetheless there are some
// cases where logging is necessary and when this file was first introduced we needed to replace all
// uses of iostream so we could see them in Android's logcat.
//
// Regular logging is done using the [Debug|Info|Warning|Error]Log() function this way:
//
//   InfoLog() << things << that << ostringstream << supports; // No need for a std::endl or "\n"
//
// It creates a LogMessage object that isn't stored anywhere and gets its destructor called
// immediately which outputs the stored ostringstream in the right place.
//
// This file also contains PROBEGL_DEBUG for "printf debugging" which works on Android and
// additionally outputs the file, line and function name. Use it this way:
//
//   // Pepper this throughout code to get a log of the execution
//   PROBEGL_DEBUG();
//
//   // Get more information
//   PROBEGL_DEBUG() << texture.GetFormat();

#include <sstream>

namespace probegl {

// Log levels mostly used to signal intent where the log message is produced and used to route
// the message to the correct output.
enum class LogSeverity {
  Debug,
  Info,
  Warning,
  Error,
};

// Essentially an ostringstream that will print itself in its destructor.
class LogMessage {
 public:
  explicit LogMessage(LogSeverity severity);
  ~LogMessage();

  LogMessage(LogMessage&& other) = default;
  auto operator=(LogMessage&& other) -> LogMessage& = default;

  template <typename T>
  auto operator<<(T&& value) -> LogMessage& {
    mStream << value;
    return *this;
  }

 private:
  LogMessage(const LogMessage& other) = delete;
  auto operator=(const LogMessage& other) -> LogMessage& = delete;

  LogSeverity mSeverity;
  std::ostringstream mStream;
};

// Short-hands to create a LogMessage with the respective severity.
auto DebugLog() -> LogMessage;
auto InfoLog() -> LogMessage;
auto WarningLog() -> LogMessage;
auto ErrorLog() -> LogMessage;

// PROBEGL_DEBUG is a helper macro that creates a DebugLog and outputs file/line/function information
auto DebugLog(const char* file, const char* function, int line) -> LogMessage;
#define PROBEGL_DEBUG() ::probegl::DebugLog(__FILE__, __func__, __LINE__)

}  // namespace probegl

#endif  // PROBEGL_LOG_H_
