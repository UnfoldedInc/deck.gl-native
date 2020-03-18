#ifndef DECKGL_JSON_JSON_TYPES_H
#define DECKGL_JSON_JSON_TYPES_H

#include "json/json.h"  // {Json::Value} (https://github.com/open-source-parsers/jsoncpp)

namespace deckgl {

// TODO - mathgl types

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
