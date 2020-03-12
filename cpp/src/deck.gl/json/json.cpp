#include "deck.gl/json.h"

#include <memory>

using namespace deckgl;

// Json::Value hello() {
//   // Json::StreamWriterBuilder builder;
//   // const std::unique_ptr<Json::StreamWriter>
//   // writer(builder.newStreamWriter());

//   Json::Value root;
//   root["hello"] = "world";
//   return root;
// }

auto JSONConfiguration::convertJson(const Json::Value& value) -> std::shared_ptr<Component> {
  switch (value.type()) {
    default:
      break;
  }
  return nullptr;
}
