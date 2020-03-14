#ifndef DECKGL_JSON_JSON_CONFIGURATION_H
#define DECKGL_JSON_JSON_CONFIGURATION_H

#include <functional>  // {std::function}
#include <map>
#include <memory>  // {std::shared_ptr}

#include "deck.gl/core.h"  // {Component}
#include "json/json.h"     // {Json::Value} (https://github.com/open-source-parsers/jsoncpp)

namespace deckgl {

class JSONConfiguration {
 public:
  using JsonValueToComponentConverter = std::function<auto(const Json::Value&)->std::shared_ptr<Component>>;

  // public members
  std::map<std::string, JsonValueToComponentConverter> classes;

  // methods
  JSONConfiguration() {}
  JSONConfiguration(const std::map<std::string, JsonValueToComponentConverter>& classes_) : classes{classes_} {}

  // parse JSON string using jsoncpp
  auto parseJson(const std::string& rawJson) -> Json::Value;

  // Convert parsed JSON into registered classes
  auto convertJson(const Json::Value&) -> std::shared_ptr<Component>;
};

}  // namespace deckgl

#endif  // DECKGL_JSON_JSON_CONFIGURATION_H
