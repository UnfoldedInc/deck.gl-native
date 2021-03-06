// Copyright (c) 2020 Unfolded, Inc.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include "./json-object.h"  // NOLINT(build/include)

#include <iostream>

#include "../converter/json-converter.h"

using namespace deckgl;

// Property

auto Property::_getPropFromJson(JSONObject* props, const Json::Value& jsonValue,
                                const JSONConverter* jsonConverter) const -> std::shared_ptr<JSONObject> {
  auto typeHint = this->typeName;
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

Properties::Properties(const std::string& className, const std::shared_ptr<Properties>& parentProps,
                       const std::vector<std::shared_ptr<Property>>& ownPropertyDefs)
    : className{className}, parent{parentProps} {
  // insert our prop types
  for (auto element : ownPropertyDefs) {
    this->_propTypeMap.insert({element->name, element});
  }

  // Insert parent's prop types
  if (parentProps) {
    const auto& parentPropertyMap = parentProps->_propTypeMap;
    this->_propTypeMap.insert(parentPropertyMap.begin(), parentPropertyMap.end());
  }
}

// JSONObject

JSONObject::JSONObject() {}

JSONObject::~JSONObject() {}

auto JSONObject::getProperties() const -> const std::shared_ptr<Properties> {
  // Can't use make_shared here as constructor is private
  return std::shared_ptr<deckgl::Properties>(new Properties{"Component", nullptr, {}});
}

void JSONObject::setPropertyFromJson(const std::string& key, const Json::Value& jsonValue,
                                     const JSONConverter* jsonConverter) {
  auto propertyType = this->getProperty(key);
  propertyType->setPropertyFromJson(this, jsonValue, jsonConverter);
}

auto JSONObject::getProperty(const std::string& key) const -> const std::shared_ptr<Property> {
  auto properties = this->getProperties();
  if (!properties) {
    throw std::logic_error("Component does not have property types");
  }

  auto propertyType = this->getProperties()->getProperty(key);
  if (!propertyType) {
    throw std::runtime_error("Prop type not found for property " + key);
  }

  return propertyType;
}

auto JSONObject::equals(const JSONObject* other) const -> bool {
  auto properties = this->getProperties();

  if (other == nullptr) {
    return false;
  }
  if (other == this) {
    return true;
  }
  if (other->getProperties()->className != properties->className) {
    return false;
  }

  for (auto element : properties->_propTypeMap) {
    // Accessing KEY from element
    std::string name = element.first;
    // Accessing VALUE from element.
    const auto propType = element.second;
    if (!propType->equals(this, other)) {
      return false;
    }
  }

  return true;
}

auto JSONObject::compare(const JSONObject* other) const -> std::optional<std::string> {
  auto properties = this->getProperties();

  if (other == nullptr) {
    return properties->className + " now is null";
  }
  if (other == this) {
    return std::nullopt;
  }
  if (other->getProperties()->className != properties->className) {
    return properties->className + " class changed";
  }

  for (auto element : properties->_propTypeMap) {
    // Accessing KEY from element
    std::string name = element.first;
    // Accessing VALUE from element.
    const auto propType = element.second;
    if (!propType->equals(this, other)) {
      return properties->className + "." + propType->name + " changed";
    }
  }

  return std::nullopt;
}

auto operator<<(std::ostream& os, const JSONObject& obj) -> std::ostream& {
  // Debug output, not for serialization
  auto properties = obj.getProperties();
  os << properties->className << "{";
  for (auto prop : properties->allProperties()) {
    auto propName = prop->getName();
    os << "\n\t" << propName << ": " << prop->toString(&obj);
  }
  os << "\n}";
  return os;
}

auto operator==(const JSONObject& lhs, const JSONObject& rhs) -> bool { return lhs.equals(&rhs); }

auto operator!=(const JSONObject& lhs, const JSONObject& rhs) -> bool { return !(lhs == rhs); }
