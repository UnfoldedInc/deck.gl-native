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

#ifndef DECKGL_JSON_JSON_OBJECT_H
#define DECKGL_JSON_JSON_OBJECT_H

#include <functional>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "../converter/json-types-mathgl.h"  // {fromJson<T>(math.gl types)}
#include "../converter/json-types.h"         // {fromJson<T>(std types)}
#include "json/json.h"                       // {Json::Value}

namespace deckgl {

// Forward declarations from other files
class JSONConverter;  // #include "../converter/json-converter.h"

class Properties;
class Property;
template <class T>
class PropertyT;

class JSONObject {
 public:
  static constexpr const char* getTypeName() { return "JSONObject"; }

  JSONObject();
  virtual ~JSONObject();

  // Returns the shared static Properties for this Prop object.
  virtual auto getProperties() const -> const Properties*;

  // Compares the contents of this prop object against another prop object
  auto equals(const JSONObject* other) const -> bool;
  auto equals(std::shared_ptr<JSONObject> other) const -> bool { return this->equals(other.get()); }
  auto compare(const JSONObject* other) const -> std::optional<std::string>;
  auto compare(std::shared_ptr<JSONObject> other) const -> std::optional<std::string> {
    return this->compare(other.get());
  }

  bool hasProperty(const std::string& key) const;

  // Sets one property on this prop object
  template <class T>
  void setProperty(const std::string& key, const T& value);

  // Set a property from a JSON value
  void setPropertyFromJson(const std::string& key, const Json::Value& jsonValue, const JSONConverter*);

  // Gets the general property type object for a key
  auto getProperty(const std::string& key) const -> const Property*;

  // Get the typed property type object for a key
  template <class T>
  auto getPropertyT(const std::string& key) const -> const PropertyT<T>*;

 private:
  std::unique_ptr<Properties> _properties;
};

class Property {
 public:
  // The name of the field (this string matches the name of the field in the C++ Prop class)
  // Note: This is intended to be initialized with string constants that have static linkage.
  const char* name;
  const char* typeName{nullptr};
  bool isList{false};

  explicit Property(const char* name_) : name{name_} {}
  virtual ~Property() {}

  auto getName() const -> std::string { return this->name; }

  virtual bool equals(const JSONObject*, const JSONObject*) const = 0;
  virtual void setPropertyFromJson(JSONObject*, const Json::Value&, const JSONConverter*) const {}

 protected:
  auto _getPropFromJson(JSONObject* props, const Json::Value& jsonValue, const JSONConverter* jsonConverter) const
      -> std::shared_ptr<JSONObject>;
  auto _getPropListFromJson(JSONObject* props, const Json::Value& jsonValue, const JSONConverter* jsonConverter) const
      -> std::list<std::shared_ptr<JSONObject>>;
};

template <class T>
class PropertyT : public Property {
 public:
  std::function<auto(JSONObject const*)->T> get;  // TODO(ib): return const T& ?
  std::function<void(JSONObject*, T)> set;
  T defaultValue;

  PropertyT<T>(const char* name_, const std::function<auto(JSONObject const*)->T>& get_,
               const std::function<void(JSONObject*, T)>& set_, const T& defaultValue_)
      : Property{name_}, get{get_}, set{set_}, defaultValue{defaultValue_} {}

  bool equals(const JSONObject* props1, const JSONObject* props2) const override {
    // Note: `equalsT` provides approximate equality for floats (avoiding rounding errors)
    return mathgl::equalsT(this->get(props1), this->get(props2));
  }
  void setPropertyFromJson(JSONObject* props, const Json::Value& jsonValue, const JSONConverter*) const override {
    this->set(props, fromJson<T>(jsonValue));
  }
};

template <class T>
struct PropertyT<std::optional<T>> : public Property {
 public:
  std::function<auto(JSONObject const*)->const std::optional<T>&> get;  // TODO(ib): return const T& ?
  std::function<void(JSONObject*, const T&)> set;
  T defaultValue;

  PropertyT(const char* name_, const std::function<auto(JSONObject const*)->const std::optional<T>&>& get_,
            const std::function<void(JSONObject*, const T&)>& set_, const T& _defaultValue)
      : Property{name_}, get{get_}, set{set_}, defaultValue{_defaultValue} {
    this->typeName = "optional";  // T::getTypeName();
  }

  bool equals(const JSONObject* props1, const JSONObject* props2) const override {
    // Note: `equalsT` provides approximate equality for floats (avoiding rounding errors)
    return mathgl::equalsT(this->get(props1), this->get(props2));
  }

  void setPropertyFromJson(JSONObject* props, const Json::Value& jsonValue,
                           const JSONConverter* jsonConverter) const override {
    this->set(props, fromJson<T>(jsonValue));
  }
};

template <class T>
struct PropertyT<std::shared_ptr<T>> : public Property {
 public:
  std::function<auto(JSONObject const*)->const std::shared_ptr<T>&> get;  // TODO(ib): return const T& ?
  std::function<void(JSONObject*, const std::shared_ptr<T>&)> set;
  std::shared_ptr<T> defaultValue;

  PropertyT(const char* name_, const std::function<auto(JSONObject const*)->const std::shared_ptr<T>&>& get_,
            const std::function<void(JSONObject*, const std::shared_ptr<T>&)>& set_)
      : Property{name_}, get{get_}, set{set_} {
    this->typeName = T::getTypeName();
  }

  bool equals(const JSONObject* props1, const JSONObject* props2) const override {
    // Note: `equalsT` provides approximate equality for floats (avoiding rounding errors)
    return mathgl::equalsT(this->get(props1), this->get(props2));
  }

  void setPropertyFromJson(JSONObject* props, const Json::Value& jsonValue,
                           const JSONConverter* jsonConverter) const override {
    auto prop = this->_getPropFromJson(props, jsonValue, jsonConverter);
    auto typedProp = std::dynamic_pointer_cast<T>(prop);
    if (!typedProp) {
      throw std::runtime_error("JSON object (in list) created but of wrong type");
    }
    this->set(props, typedProp);
  }
};

template <class T>
struct PropertyT<std::list<std::shared_ptr<T>>> : public Property {
 public:
  std::function<auto(JSONObject const*)->const std::list<std::shared_ptr<T>>&> get;  // TODO(ib): return const T& ?
  std::function<void(JSONObject*, const std::list<std::shared_ptr<T>>&)> set;
  std::list<std::shared_ptr<T>> defaultValue;

  PropertyT(const char* name_, const std::function<auto(JSONObject const*)->const std::list<std::shared_ptr<T>>&>& get_,
            const std::function<void(JSONObject*, const std::list<std::shared_ptr<T>>&)>& set_)
      : Property{name_}, get{get_}, set{set_} {
    this->typeName = T::getTypeName();
    this->isList = true;
  }

  bool equals(const JSONObject* props1, const JSONObject* props2) const override {
    // Note: `equalsT` provides approximate equality for floats (avoiding rounding errors)
    return mathgl::equalsT(this->get(props1), this->get(props2));
  }

  void setPropertyFromJson(JSONObject* props, const Json::Value& jsonValue,
                           const JSONConverter* jsonConverter) const override {
    auto propsList = this->_getPropListFromJson(props, jsonValue, jsonConverter);
    std::list<std::shared_ptr<T>> list;
    for (auto props : propsList) {
      auto typedProp = std::dynamic_pointer_cast<T>(props);
      if (!typedProp) {
        throw std::runtime_error("JSON object (in list) created but of wrong type");
      }
      list.push_back(typedProp);
    }
    this->set(props, list);
  }
};

class Properties {
  friend class JSONObject;

 public:
  // static methods
  template <typename JSONObjectT>
  static auto from(const std::string& className, const std::vector<const Property*>& properties) -> deckgl::Properties {
    typename JSONObjectT::super parentProps;
    return Properties{className, parentProps.getProperties(), properties};
  }

  // public members
  const std::string className;
  const Properties* parent;

  // methods
  bool hasProp(const std::string& key) const { return this->_propTypeMap.count(key) == 1; }
  auto getProperty(const std::string& key) const -> const Property* {
    auto searchIterator = this->_propTypeMap.find(key);
    if (searchIterator == this->_propTypeMap.end()) {
      throw std::runtime_error("No such property: " + className + "." + key);
    }
    return searchIterator->second;
  }
  auto allProperties() const -> std::vector<const Property*> {
    std::vector<const Property*> properties;
    properties.reserve(this->_propTypeMap.size());
    for (auto prop : _propTypeMap) {
      properties.push_back(prop.second);
    }
    return properties;
  }

 private:
  Properties(const std::string& className, const Properties* parentProps, const std::vector<const Property*>&);

  std::map<const std::string, const Property*> _propTypeMap;
};

// JSONObject inline members

inline bool JSONObject::hasProperty(const std::string& key) const { return this->getProperties()->hasProp(key); }

template <class T>
void JSONObject::setProperty(const std::string& key, const T& value) {
  std::cout << "setProperty: getProperties" << std::endl;
  auto properties = this->getProperties();
  if (!properties) {
    throw std::logic_error("Props: No prop types found");
  }
  std::cout << "setProperty: getProperty" << std::endl;
  if (auto propType = this->getProperties()->getProperty(key)) {
    std::cout << "setProperty: dynamic_cast" << std::endl;
    if (auto propTypeT = dynamic_cast<const PropertyT<T>*>(propType)) {
      std::cout << "setting prop" << std::endl;
      propTypeT->set(this, value);
    }
  }
}

template <class T>
auto JSONObject::getPropertyT(const std::string& key) const -> const PropertyT<T>* {
  auto propertyType = this->getProperty(key);
  auto propTypeT = dynamic_cast<const PropertyT<T>*>(propertyType);
  if (!propTypeT) {
    throw std::runtime_error("property type mismatch for " + key);
  }
  return propTypeT;
}

}  // namespace deckgl

#endif  // DECKGL_JSON_JSON_OBJECT_H
