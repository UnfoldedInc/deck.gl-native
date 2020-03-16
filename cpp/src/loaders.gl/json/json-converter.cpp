#include "./json-converter.h"

#include <arrow/api.h>
#include <arrow/json/api.h>

#include <iostream>
#include <memory>

using namespace loadersgl;
using namespace deckgl;

auto JSONConverter::parseJson(const std::string &rawJson) -> Json::Value {
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

auto JSONConverter::convertJson(const Json::Value &value) -> std::shared_ptr<Props> {
  this->_traverseJson(value,
                      [=](const std::string &key, const Json::Value) -> std::shared_ptr<Props> { return nullptr; });
  return nullptr;
}

auto JSONConverter::loadTable(const std::shared_ptr<arrow::io::InputStream> input) -> std::shared_ptr<arrow::Table> {
  arrow::Status status;
  arrow::MemoryPool *pool = arrow::default_memory_pool();

  auto readOptions = arrow::json::ReadOptions::Defaults();
  auto parseOptions = arrow::json::ParseOptions::Defaults();

  // Instantiate TableReader from input stream and options
  std::shared_ptr<arrow::json::TableReader> reader;
  status = arrow::json::TableReader::Make(pool, input, readOptions, parseOptions, &reader);
  if (!status.ok()) {
    // Ideally we provide additional context here somehow...
    throw std::runtime_error("Cannot instantiate TableReader");
  }

  std::shared_ptr<arrow::Table> table;
  // Read table from JSON file
  status = reader->Read(&table);
  if (!status.ok()) {
    // Ideally we provide additional context here somehow...
    throw std::runtime_error("An error has occured while parsing JSON data");
  }

  return table;
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
      break;
    //   for (auto iter : value) {
    //     const wstring &k = iter.first;
    //     const Json::Calue &val = iter.second;
    //     traverseJson(val, visitor, k, level + 1);
    //   }
    //   break;
    case Json::ValueType::arrayValue:
      for (auto it : value) {
        this->_traverseJson(it, visitor, key, level + 1);
      }
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

  // std::cout << "Converting class \"" << className << "\"" << std::endl;

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
