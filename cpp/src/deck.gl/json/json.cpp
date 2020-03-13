#include "deck.gl/json.h"

#include <memory>

using namespace deckgl;

auto JSONConfiguration::convertJson(const Json::Value& value) -> std::shared_ptr<Component> {
  switch (value.type()) {
    default:
      break;
  }
  return nullptr;
}
