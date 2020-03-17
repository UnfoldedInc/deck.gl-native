#include "./json-types.h"

#include "./json-types-mathgl.h"

namespace deckgl {

// standard type converters

template <>
auto fromJson<bool>(const Json::Value &jsonValue) -> bool {
  if (!jsonValue.isBool()) {
    throw std::runtime_error("cannot convert JSON to string");
  }
  return jsonValue.asInt();
}

template <>
auto fromJson<int>(const Json::Value &jsonValue) -> int {
  if (!jsonValue.isInt()) {
    throw std::runtime_error("cannot convert JSON to string");
  }
  return jsonValue.asInt();
}

template <>
auto fromJson<double>(const Json::Value &jsonValue) -> double {
  if (!jsonValue.isDouble()) {
    throw std::runtime_error("cannot convert JSON to string");
  }
  return jsonValue.asDouble();
}

template <>
auto fromJson<float>(const Json::Value &jsonValue) -> float {
  if (!jsonValue.isDouble()) {
    throw std::runtime_error("cannot convert JSON to string");
  }
  return static_cast<float>(jsonValue.asDouble());
}

template <>
auto fromJson<std::string>(const Json::Value &jsonValue) -> std::string {
  if (!jsonValue.isString()) {
    throw std::runtime_error("cannot convert JSON to string");
  }
  return jsonValue.asString();
}

// math.gl type converters

template <>
auto fromJson<mathgl::Vector3<double>>(const Json::Value &jsonValue) -> mathgl::Vector3<double> {
  if (jsonValue.isArray() && jsonValue.size() == 3) {
    return mathgl::Vector3<double>(fromJson<double>(jsonValue[0]), fromJson<double>(jsonValue[1]),
                                   fromJson<double>(jsonValue[2]));
  }
  throw std::runtime_error("Cannot convert JSON to Vector3<double>");
}

template <>
auto fromJson<mathgl::Vector3<float>>(const Json::Value &jsonValue) -> mathgl::Vector3<float> {
  if (jsonValue.isArray() && jsonValue.size() == 3) {
    return mathgl::Vector3<float>(fromJson<float>(jsonValue[0]), fromJson<float>(jsonValue[1]),
                                  fromJson<float>(jsonValue[2]));
  }
  throw std::runtime_error("Cannot convert JSON to Vector3<float>");
}

template <>
auto fromJson<mathgl::Matrix4<double>>(const Json::Value &jsonValue) -> mathgl::Matrix4<double> {
  if (jsonValue.isArray() && jsonValue.size() == 16) {
    return mathgl::Matrix4<double>(
        fromJson<double>(jsonValue[0]), fromJson<double>(jsonValue[1]), fromJson<double>(jsonValue[2]),
        fromJson<double>(jsonValue[3]), fromJson<double>(jsonValue[4]), fromJson<double>(jsonValue[5]),
        fromJson<double>(jsonValue[6]), fromJson<double>(jsonValue[7]), fromJson<double>(jsonValue[8]),
        fromJson<double>(jsonValue[9]), fromJson<double>(jsonValue[10]), fromJson<double>(jsonValue[11]),
        fromJson<double>(jsonValue[12]), fromJson<double>(jsonValue[13]), fromJson<double>(jsonValue[14]),
        fromJson<double>(jsonValue[15]));
  }
  throw std::runtime_error("Cannot convert JSON to Matrix4<double>");
}

template <>
auto fromJson<mathgl::Matrix4<float>>(const Json::Value &jsonValue) -> mathgl::Matrix4<float> {
  if (jsonValue.isArray() && jsonValue.size() == 16) {
    return mathgl::Matrix4<float>(
        fromJson<float>(jsonValue[0]), fromJson<float>(jsonValue[1]), fromJson<float>(jsonValue[2]),
        fromJson<float>(jsonValue[3]), fromJson<float>(jsonValue[4]), fromJson<float>(jsonValue[5]),
        fromJson<float>(jsonValue[6]), fromJson<float>(jsonValue[7]), fromJson<float>(jsonValue[8]),
        fromJson<float>(jsonValue[9]), fromJson<float>(jsonValue[10]), fromJson<float>(jsonValue[11]),
        fromJson<float>(jsonValue[12]), fromJson<float>(jsonValue[13]), fromJson<float>(jsonValue[14]),
        fromJson<float>(jsonValue[15]));
  }
  throw std::runtime_error("Cannot convert JSON to Matrix4<float>");
}

}  // namespace deckgl
