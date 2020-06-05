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

#ifndef PROBEGL_CORE_ERROR_H
#define PROBEGL_CORE_ERROR_H

#include <functional>
#include <memory>
#include <optional>
#include <string>

namespace probegl {

using Error = std::optional<std::string>;

template <typename T>
auto catchError(std::function<std::shared_ptr<T>()> expression, Error& error) noexcept -> std::shared_ptr<T> {
  error.reset();

  try {
    return expression();
  } catch (const std::exception& ex) {
    error = ex.what();
    return nullptr;
  } catch (...) {
    error = "Operation failed with unknown error";
    return nullptr;
  }
}

template <typename T>
auto catchError(std::function<T()> expression, Error& error) noexcept -> std::shared_ptr<T> {
  return catchError<T>([expression]() { return std::make_shared<T>(expression()); }, error);
}

void catchError(std::function<void()> expression, Error& error) noexcept;

}  // namespace probegl

#endif  // PROBEGL_CORE_ERROR_H
