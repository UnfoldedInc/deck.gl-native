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

auto JSONConverter::convertJson(const Json::Value &value) const -> std::shared_ptr<Component::Props> {
  this->_traverseJson(
      value, [=](const std::string &key, const Json::Value) -> std::shared_ptr<Component::Props> { return nullptr; });
  return nullptr;
}

auto JSONConverter::_traverseJson(const Json::Value &value, std::function<Visitor> visitor, const std::string &key,
                                  int level) const -> std::shared_ptr<Component::Props> {
  switch (value.type()) {
    case Json::ValueType::objectValue:
      return this->_convertClassProps(value, visitor, level);

    /*
    case Json::ValueType::arrayValue:
      for (auto it : value) {
        this->_traverseJson(it, visitor, key, level + 1);
      }
      // TODO - return std::list
      break;
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
    case Json::ValueType::nullValue:
      break;
    */
    default:
      throw std::runtime_error("JSON style is not an object");
  }
}

auto JSONConverter::_convertClassProps(const Json::Value &object, std::function<Visitor>, int level) const
    -> std::shared_ptr<Component::Props> {
  auto className = object["@@type"].asString();

  auto findIterator = this->classes.find(className);
  if (findIterator == this->classes.end()) {
    throw std::runtime_error("JSON contains unknown class with @@type: \"" + className + "\"");
  }

  auto classConverter = findIterator->second;
  auto props = classConverter(object);
  if (!props) {
    throw std::runtime_error("JSON class conversion failed for @@type: \"" + className + "\"");
  }

  for (auto it = object.begin(); it != object.end(); ++it) {
    auto key = it.key();
    auto value = *it;  // Json::Value
    if (key != "@@type") {
      if (props->hasProperty(key.asString())) {
        props->setPropertyFromJson(key.asString(), value, this);
      } else {
        std::cout << "ignoring prop " << key << std::endl;
      }
    }
  }

  return props;
}
