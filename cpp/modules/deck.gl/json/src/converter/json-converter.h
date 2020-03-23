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

#include <functional>  // {std::function}
#include <map>
#include <memory>  // {std::shared_ptr}
#include <string>

#include "../json-object/json-object.h"  // {JSONObject}
#include "json/json.h"                   // {Json::Value} (https://github.com/open-source-parsers/jsoncpp)

namespace deckgl {

class JSONConverter {
 public:
  using JsonValueToComponentConverter = std::function<auto(const Json::Value &)->std::shared_ptr<JSONObject>>;

  // public members
  std::map<std::string, JsonValueToComponentConverter> classes;

  // methods
  JSONConverter() {}
  explicit JSONConverter(const std::map<std::string, JsonValueToComponentConverter> &classes_) : classes{classes_} {}

  // parse JSON string using jsoncpp
  auto parseJson(const std::string &rawJson) -> Json::Value;

  // Convert parsed JSON into registered classes
  auto convertJson(const Json::Value &) const -> std::shared_ptr<JSONObject>;
  auto convertClass(const Json::Value &, const std::string &typeHint) const -> std::shared_ptr<JSONObject>;

 private:
  using Visitor = auto(const std::string &key, const Json::Value) -> std::shared_ptr<JSONObject>;

  auto _traverseJson(const Json::Value &, std::function<Visitor>, const std::string &key = "", int level = 0) const
      -> std::shared_ptr<JSONObject>;
  auto _convertClassProps(const Json::Value &, const std::string &typeHint, std::function<Visitor>, int level) const
      -> std::shared_ptr<JSONObject>;
};

extern JSONConverter jsonConverter;

}  // namespace deckgl

#endif  // DECKGL_JSON_JSON_CONVERTER_H
