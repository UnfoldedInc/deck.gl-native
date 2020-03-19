#include "./json-object.h"  // {JSONObject}

#include <iostream>

#include "../converter/json-converter.h"  // {JSONConverter}

using namespace deckgl;

// Property

auto Property::_getPropFromJson(JSONObject* props, const Json::Value& jsonValue,
                                const JSONConverter* jsonConverter) const -> std::shared_ptr<JSONObject> {
  auto typeHint = this->typeName;
  std::cout << "getting prop" << typeHint << std::endl;
  std::shared_ptr<JSONObject> propsObject = {jsonConverter->convertClass(jsonValue, typeHint)};
  return propsObject;
}

auto Property::_getPropListFromJson(JSONObject* props, const Json::Value& jsonValue,
                                    const JSONConverter* jsonConverter) const
    -> std::list<std::shared_ptr<JSONObject>> {
  auto typeHint = props->getProperties()->className;
  if (jsonValue.isArray()) {
    std::list<std::shared_ptr<JSONObject>> propsList;
    for (Json::Value::ArrayIndex i = 0; i < jsonValue.size(); ++i) {
      std::shared_ptr<JSONObject> props = {jsonConverter->convertClass(jsonValue[i], typeHint)};
      propsList.push_back(props);
    }
    return propsList;
  }
  throw std::runtime_error("Cannot convert JSON to list: " + this->getName());
}

// Properties

Properties::Properties(const std::string& className_, const Properties* parentProperties,
                       const std::vector<const Property*>& ownPropertyDefs)
    : className{className_}, parent{parentProperties} {
  // insert our prop types
  for (auto element : ownPropertyDefs) {
    this->_propTypeMap.insert({element->name, element});
  }

  // Insert parent's prop types
  if (parentProperties) {
    const auto& parentPropertyMap = parentProperties->_propTypeMap;
    this->_propTypeMap.insert(parentPropertyMap.begin(), parentPropertyMap.end());
  }
}

// JSONObject

JSONObject::JSONObject() {}

JSONObject::~JSONObject() {}

auto JSONObject::getProperties() const -> const Properties* {
  static Properties propTypes{"Component", nullptr, std::vector<const Property*>{}};
  return &propTypes;
}

void JSONObject::setPropertyFromJson(const std::string& key, const Json::Value& jsonValue,
                                     const JSONConverter* jsonConverter) {
  auto propertyType = this->getProperty(key);
  propertyType->setPropertyFromJson(this, jsonValue, jsonConverter);
}

auto JSONObject::getProperty(const std::string& key) const -> const Property* {
  // std::cout << "getProperties" << std::endl;
  auto propTypes = this->getProperties();
  if (!propTypes) {
    throw std::logic_error("Component does not have property types");
  }

  // std::cout << "getProperty" << std::endl;
  auto propertyType = this->getProperties()->getProperty(key);
  if (!propertyType) {
    throw std::runtime_error("Prop type not found for property " + key);
  }

  return propertyType;
}

auto JSONObject::equals(const JSONObject* other) -> bool {
  auto propTypes = this->getProperties();

  for (auto element : propTypes->_propTypeMap) {
    // Accessing KEY from element
    std::string name = element.first;
    // Accessing VALUE from element.
    const Property* propType = element.second;
    if (!propType->equals(this, other)) {
      return false;
    }
  }

  return true;
}

auto JSONObject::compare(const JSONObject* other) -> std::optional<std::string> {
  auto propTypes = this->getProperties();

  for (auto element : propTypes->_propTypeMap) {
    // Accessing KEY from element
    std::string name = element.first;
    // Accessing VALUE from element.
    const Property* propType = element.second;
    if (!propType->equals(this, other)) {
      return std::string{propTypes->className} + propType->name + " changed";
    }
  }

  return std::nullopt;
}
