#include "./json-converter.h"

#include <iostream>
#include <memory>

using namespace deckgl;

auto JSONConverter::parseJson(const std::string &rawJson) -> Json::Value {
  Json::CharReaderBuilder builder;
  const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());

  Json::Value rootValue;
  JSONCPP_STRING err;
  bool parsingSuccessful = reader->parse(rawJson.c_str(), rawJson.c_str() + rawJson.length(), &rootValue, &err);
  if (!parsingSuccessful) {
    throw std::runtime_error("JSON parsing failed: " + err);
  }
  return rootValue;
}

auto JSONConverter::convertJson(const Json::Value &value) -> std::shared_ptr<Props> {
  this->_traverseJson(value,
                      [=](const std::string &key, const Json::Value) -> std::shared_ptr<Props> { return nullptr; });
  return nullptr;
}

auto JSONConverter::_traverseJson(const Json::Value &value, std::function<Visitor> visitor, const std::string &key,
                                  int level) -> std::shared_ptr<Props> {
  switch (value.type()) {
    case Json::ValueType::realValue:
      return visitor(key, value.asDouble());
      break;
    case Json::ValueType::intValue:
      return visitor(key, value.asInt());
      break;
    case Json::ValueType::uintValue:
      return visitor(key, value.asUInt());
      break;
    case Json::ValueType::booleanValue:
      return visitor(key, value.asBool());
      break;
    case Json::ValueType::stringValue:
      return visitor(key, value.asString());
      break;
    case Json::ValueType::objectValue:
      if (!value.isMember("@@type")) {
        throw std::runtime_error("JSON contains non-typed object at " + key);
      }
      if (!value["@@type"].isString()) {
        throw std::runtime_error("JSON contains object with non-string type at " + key);
      }
      return this->_convertClassProps(value, visitor, level);
    case Json::ValueType::arrayValue:
      for (auto it : value) {
        this->_traverseJson(it, visitor, key, level + 1);
      }
      // TODO - return std::list
      break;
    case Json::ValueType::nullValue:
      break;
  }

  return nullptr;
}

void setPropToJsonValue(std::shared_ptr<Component::Props> props, const std::string &key, Json::Value &value) {
  switch (value.type()) {
    case Json::ValueType::realValue:
      props->setProperty(key, value.asDouble());
      break;
    case Json::ValueType::intValue:
      props->setProperty(key, value.asInt());
      break;
    case Json::ValueType::uintValue:
      props->setProperty(key, value.asUInt());
      break;
    case Json::ValueType::booleanValue:
      props->setProperty(key, value.asBool());
      break;
    case Json::ValueType::stringValue:
      props->setProperty(key, value.asString());
      break;
    case Json::ValueType::objectValue:
    case Json::ValueType::arrayValue:
    case Json::ValueType::nullValue:
      // TODO
      break;
  }
}

auto JSONConverter::_convertClassProps(const Json::Value &object, std::function<Visitor> visitor, int level)
    -> std::shared_ptr<Props> {
  auto className = object["@@type"].asString();

  auto classConverter = this->classes[className];
  if (!classConverter) {
    throw std::runtime_error("JSON contains unknown class with @@type: \"" + className + "\"");
  }

  auto props = classConverter(object);
  if (!props) {
    // throw std::runtime_error("JSON class conversion failed for @@type: \"" + className + "\"");
    return nullptr;
  }

  for (auto it = object.begin(); it != object.end(); ++it) {
    auto key = it.key();
    auto value = *it;  // Json::Value
    if (key != "@@type") {
      setPropToJsonValue(props, key.asString(), value);
    }
  }

  return props;
}
