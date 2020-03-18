#ifndef DECKGL_JSON_JSON_TYPES_MATHGL_H
#define DECKGL_JSON_JSON_TYPES_MATHGL_H

#include "./json-types.h"
#include "math.gl/core.h"

namespace deckgl {

template <>
auto fromJson<mathgl::Vector3<double>>(const Json::Value &jsonValue) -> mathgl::Vector3<double>;

template <>
auto fromJson<mathgl::Vector3<float>>(const Json::Value &jsonValue) -> mathgl::Vector3<float>;

template <>
auto fromJson<mathgl::Matrix4<double>>(const Json::Value &jsonValue) -> mathgl::Matrix4<double>;

template <>
auto fromJson<mathgl::Matrix4<float>>(const Json::Value &jsonValue) -> mathgl::Matrix4<float>;

}  // namespace deckgl

#endif  // DECKGL_JSON_JSON_TYPES_H
