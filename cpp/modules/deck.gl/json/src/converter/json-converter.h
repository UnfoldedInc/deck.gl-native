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

#ifndef DECKGL_JSON_JSON_CONVERTER_H
#define DECKGL_JSON_JSON_CONVERTER_H

#include <json/json.h>

#include <functional>
#include <map>
#include <memory>
#include <string>

#include "../json-object/json-object.h"

namespace deckgl {

/// \brief JSONConverter enables parsing raw JSON payloads into a hierarchy of JSONObject instances.
class JSONConverter {
 public:
  using JsonValueToComponentConverter = std::function<auto(const Json::Value& value)->std::shared_ptr<JSONObject>>;

  JSONConverter() {}
  explicit JSONConverter(const std::map<std::string, JsonValueToComponentConverter>& classes) : classes{classes} {}

  /// \brief Parses rawJson into one of the registered classes, which are infered either by root property "@@type",
  /// or the given typeHint.
  /// \param rawJson JSON payload to parse.
  /// \param typeHint Name of the type under which the class was registered, if one wasn't provided in the payload.
  /// \returns JSONObject instance that represents the root object.
  auto convertJson(const std::string& rawJson, const std::string& typeHint = "") const -> std::shared_ptr<JSONObject>;

  // parse JSON string using jsoncpp

  /// \brief Parses rawJson into a value object.
  /// \param rawJson JSON payload to parse.
  /// \returns jsoncpp Value that represents the root object.
  auto parseJson(const std::string& rawJson) const -> Json::Value;

  // Convert parsed JSON into registered classes

  /// \brief Converts a parsed jsoncpp Value into a JSONObject, using typeHint to determine the underlying class.
  /// \param value Value to convert.
  /// \param typeHint Class that the value represents.
  /// \returns JSONObject instance that represents the root object.
  auto convertClass(const Json::Value& value, const std::string& typeHint) const -> std::shared_ptr<JSONObject>;

  /// \brief A map of registered classes that the converter uses when parsing.
  std::map<std::string, JsonValueToComponentConverter> classes;

 private:
  using Visitor = auto(const std::string& key, const Json::Value value) -> std::shared_ptr<JSONObject>;

  auto _traverseJson(const Json::Value& value, std::function<Visitor> visitor, const std::string& key = "",
                     int level = 0) const -> std::shared_ptr<JSONObject>;
  auto _convertClassProps(const Json::Value& value, const std::string& typeHint, std::function<Visitor>,
                          int level) const -> std::shared_ptr<JSONObject>;
};

extern JSONConverter jsonConverter;

}  // namespace deckgl

#endif  // DECKGL_JSON_JSON_CONVERTER_H
