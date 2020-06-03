// Copyright (c) 2020 Unfolded, Inc.
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

#ifndef DECKGL_JSON_JSON_TYPES_H
#define DECKGL_JSON_JSON_TYPES_H

#include <json/json.h>

#include <string>

namespace deckgl {

// TODO(ib) - mathgl types

// template <typename T>
// struct Type {};

// template <>
// struct Type<bool> {
//   static constexpr const char *name{"bool"};
// };

// template <>
// struct Type<int> {
//   static constexpr const char *name{"int"};
// };

template <class T>
auto fromJson(const Json::Value &) -> T;

template <>
auto fromJson<bool>(const Json::Value &jsonValue) -> bool;

template <>
auto fromJson<int>(const Json::Value &jsonValue) -> int;

template <>
auto fromJson<double>(const Json::Value &jsonValue) -> double;

template <>
auto fromJson<float>(const Json::Value &jsonValue) -> float;

template <>
auto fromJson<std::string>(const Json::Value &jsonValue) -> std::string;

}  // namespace deckgl

#endif  // DECKGL_JSON_JSON_TYPES_H
