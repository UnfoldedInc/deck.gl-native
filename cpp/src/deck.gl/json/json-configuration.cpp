#include "./json-configuration.h"

#include <memory>

using namespace deckgl;

auto JSONConfiguration::parseJson(const std::string& rawJson) -> Json::Value {
  // Json::Reader reader;
  // bool parsingSuccessful = reader.parse(jsonData, root);  // parse process
  // std::cout << "Failed to parse JSON" << reader.getFormattedErrorMessages();

  Json::CharReaderBuilder builder;
  const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());

  Json::Value rootValue;
  JSONCPP_STRING err;
  bool parsingSuccessful = reader->parse(rawJson.c_str(), rawJson.c_str() + rawJson.length(), &rootValue, &err);
  if (!parsingSuccessful) {
    throw new std::runtime_error("JSON parsing failed");
  }
  return rootValue;
}

auto JSONConfiguration::convertJson(const Json::Value& value) -> std::shared_ptr<Component> {
  switch (value.type()) {
    default:
      break;
  }
  return nullptr;
}
