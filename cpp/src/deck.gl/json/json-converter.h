#ifndef DECKGL_JSON_JSON_CONFIGURATION_H
#define DECKGL_JSON_JSON_CONFIGURATION_H

#include <functional>  // {std::function}
#include <map>
#include <memory>  // {std::shared_ptr}

#include "deck.gl/core.h"  // {Component} // TODO - this is a "circular" dependency
#include "json/json.h"     // {Json::Value} (https://github.com/open-source-parsers/jsoncpp)

namespace deckgl {

class JSONConverter {
 public:
  using JsonValueToComponentConverter = std::function<auto(const Json::Value &)->std::shared_ptr<Props>>;

  // public members
  std::map<std::string, JsonValueToComponentConverter> classes;

  // methods
  JSONConverter() {}
  JSONConverter(const std::map<std::string, JsonValueToComponentConverter> &classes_) : classes{classes_} {}

  // parse JSON string using jsoncpp
  auto parseJson(const std::string &rawJson) -> Json::Value;

  // Convert parsed JSON into registered classes
  auto convertJson(const Json::Value &) -> std::shared_ptr<Props>;

 private:
  using Visitor = auto(const std::string &key, const Json::Value) -> std::shared_ptr<Props>;

  auto _traverseJson(const Json::Value &, std::function<Visitor>, const std::string &key = "", int level = 0)
      -> std::shared_ptr<Props>;
  auto _convertClassProps(const Json::Value &, std::function<Visitor>, int level) -> std::shared_ptr<Props>;
};

}  // namespace deckgl

#endif  // DECKGL_JSON_JSON_CONFIGURATION_H
