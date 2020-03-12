#ifndef DECKGL_JSON_H
#define DECKGL_JSON_H

#include <functional>  // {std::function}
#include <map>
#include <memory>  // {std::shared_ptr}

#include "deck.gl/core.h"  // {Component}
#include "json/json.h"     // {Json::Value} (https://github.com/open-source-parsers/jsoncpp)

namespace deckgl {

class JSONConfiguration {
 public:
  using JsonValueToComponentConverter = std::function<auto(const Json::Value&)->std::shared_ptr<Component>>;

  std::map<std::string, JsonValueToComponentConverter> classes;

  JSONConfiguration(const std::map<std::string, JsonValueToComponentConverter>& classes_) : classes{classes_} {}

  auto convertJson(const Json::Value&) -> std::shared_ptr<Component>;
};

}  // namespace deckgl

#endif  // DECKGL_JSON_H
