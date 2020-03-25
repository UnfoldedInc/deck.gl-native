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

#include "./log.h"

#include "./assert.h"
#include "./platform.h"

#include <cstdio>

#if defined(DAWN_PLATFORM_ANDROID)
# include <android/log.h>
#endif

namespace dawn {

    namespace {

        const char* SeverityName(LogSeverity severity) {
            switch (severity) {
                case LogSeverity::Debug:
                    return "Debug";
                case LogSeverity::Info:
                    return "Info";
                case LogSeverity::Warning:
                    return "Warning";
                case LogSeverity::Error:
                    return "Error";
                default:
                    UNREACHABLE();
                    return "";
            }
        }

#if defined(DAWN_PLATFORM_ANDROID)
        android_LogPriority AndroidLogPriority(LogSeverity severity) {
            switch (severity) {
                case LogSeverity::Debug:
                    return ANDROID_LOG_INFO;
                case LogSeverity::Info:
                    return ANDROID_LOG_INFO;
                case LogSeverity::Warning:
                    return ANDROID_LOG_WARN;
                case LogSeverity::Error:
                    return ANDROID_LOG_ERROR;
                default:
                    UNREACHABLE();
                    return ANDROID_LOG_ERROR;
            }
        }
#endif  // defined(DAWN_PLATFORM_ANDROID)

    }  // anonymous namespace

    LogMessage::LogMessage(LogSeverity severity) : mSeverity(severity) {
    }

    LogMessage::~LogMessage() {
        std::string fullMessage = mStream.str();

        // If this message has been moved, its stream is empty.
        if (fullMessage.empty()) {
            return;
        }

        const char* severityName = SeverityName(mSeverity);

        FILE* outputStream = stdout;
        if (mSeverity == LogSeverity::Warning || mSeverity == LogSeverity::Error) {
            outputStream = stderr;
        }

#if defined(DAWN_PLATFORM_ANDROID)
        android_LogPriority androidPriority = AndroidLogPriority(mSeverity);
        __android_log_print(androidPriority, "Dawn", "%s: %s\n", severityName, fullMessage.c_str());
#else   // defined(DAWN_PLATFORM_ANDROID)
        // Note: we use fprintf because <iostream> includes static initializers.
        fprintf(outputStream, "%s: %s\n", severityName, fullMessage.c_str());
        fflush(outputStream);
#endif  // defined(DAWN_PLATFORM_ANDROID)
    }

    LogMessage DebugLog() {
        return {LogSeverity::Debug};
    }

    LogMessage InfoLog() {
        return {LogSeverity::Info};
    }

    LogMessage WarningLog() {
        return {LogSeverity::Warning};
    }

    LogMessage ErrorLog() {
        return {LogSeverity::Error};
    }

    LogMessage DebugLog(const char* file, const char* function, int line) {
        LogMessage message = DebugLog();
        message << file << ":" << line << "(" << function << ")";
        return message;
    }

}  // namespace dawn
