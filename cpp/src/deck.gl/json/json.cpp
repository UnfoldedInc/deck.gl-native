#include "deck.gl/json.h"
#include <memory>

Json::Value hello() {
  Json::Value root;
  Json::StreamWriterBuilder builder;
  const std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());

  root["hello"] = "world";
  return root;
}
