#include "deck.gl/json.h"

#include <memory>

Json::Value hello() {
  // Json::StreamWriterBuilder builder;
  // const std::unique_ptr<Json::StreamWriter>
  // writer(builder.newStreamWriter());

  Json::Value root;
  root["hello"] = "world";
  return root;
}
