#ifndef DECKGL_JSON_COMPONENT_H
#define DECKGL_JSON_COMPONENT_H

#include <functional>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <string>

#include "../converter/json-types-mathgl.h"  // {fromJson<T>(math.gl types)}
#include "../converter/json-types.h"         // {fromJson<T>(std types)}
#include "json/json.h"                       // {Json::Value}

namespace deckgl {

// Forward declarations from other files
class JSONConverter;  // #include "../converter/json-converter.h"

class Component {
 public:
  class Props;
  virtual ~Component() {}
};

class PropertyType {
 public:
  // The name of the field (this string matches the name of the field in the C++ Prop class)
  // Note: This is intended to be initialized with string constants that have static linkage.
  const char* name;

  PropertyType(const char* name_) : name{name_} {}
  virtual ~PropertyType() {}

  auto getName() const -> std::string { return this->name; }

  virtual bool equals(const Component::Props*, const Component::Props*) const = 0;
  virtual void setPropertyFromJson(Component::Props*, const Json::Value&, const JSONConverter*) const {}

 protected:
  auto _getPropListFromJson(Component::Props* props, const Json::Value& jsonValue,
                            const JSONConverter* jsonConverter) const -> std::list<std::shared_ptr<Component::Props>>;
};

template <class T>
struct PropertyTypeT : public PropertyType {
 public:
  std::function<auto(Component::Props const*)->T> get;  // TODO return const T& ?
  std::function<void(Component::Props*, T)> set;
  T defaultValue;

  PropertyTypeT<T>(const char* name_, const std::function<auto(Component::Props const*)->T>& get_,
                   const std::function<void(Component::Props*, T)>& set_, const T& defaultValue_)
      : PropertyType{name_}, get{get_}, set{set_}, defaultValue{defaultValue_} {}

  bool equals(const Component::Props* props1, const Component::Props* props2) const override {
    // Note: `equalsT` provides approximate equality for floats (avoiding rounding errors)
    return mathgl::equalsT(this->get(props1), this->get(props2));
  }
  void setPropertyFromJson(Component::Props* props, const Json::Value& jsonValue, const JSONConverter*) const override {
    this->set(props, fromJson<T>(jsonValue));
  }
};

template <class T>
struct PropertyTypeT<std::list<std::shared_ptr<T>>> : public PropertyType {
 public:
  std::function<auto(Component::Props const*)->const std::list<std::shared_ptr<T>>&> get;  // TODO return const T& ?
  std::function<void(Component::Props*, const std::list<std::shared_ptr<T>>&)> set;
  std::list<std::shared_ptr<T>> defaultValue;

  PropertyTypeT(const char* name_,
                const std::function<auto(Component::Props const*)->const std::list<std::shared_ptr<T>>&>& get_,
                const std::function<void(Component::Props*, const std::list<std::shared_ptr<T>>&)>& set_)
      : PropertyType{name_}, get{get_}, set{set_} {}

  bool equals(const Component::Props* props1, const Component::Props* props2) const override {
    // Note: `equalsT` provides approximate equality for floats (avoiding rounding errors)
    return mathgl::equalsT(this->get(props1), this->get(props2));
  }

  void setPropertyFromJson(Component::Props* props, const Json::Value& jsonValue,
                           const JSONConverter* jsonConverter) const override {
    auto propsList = this->_getPropListFromJson(props, jsonValue, jsonConverter);
    std::list<std::shared_ptr<T>> list;
    for (auto props : propsList) {
      auto ptr = dynamic_cast<T*>(props.get());
      // TODO - throw error if cast fails
      list.push_back(std::shared_ptr<T>{ptr});
    }
    this->set(props, list);
  }
};

class PropertyTypes {
  friend class Component::Props;

 public:
  // static methods
  template <typename ComponentT>
  static auto from(const std::string& className, const std::vector<const PropertyType*>& propTypes)
      -> deckgl::PropertyTypes {
    typename ComponentT::super::Props parentProps;
    return PropertyTypes{className, parentProps.getPropertyTypes(), propTypes};
  }

  // public members
  const std::string className;
  const PropertyTypes* parent;

  // methods
  bool hasProp(const std::string& key) const { return this->_propTypeMap.count(key) == 1; }
  auto getPropertyType(const std::string& key) const -> const PropertyType* {
    auto searchIterator = this->_propTypeMap.find(key);
    if (searchIterator == this->_propTypeMap.end()) {
      throw std::runtime_error("No such property: " + className + "." + key);
    }
    return searchIterator->second;
  }

 private:
  PropertyTypes(const std::string& className, const PropertyTypes* parentProps,
                const std::vector<const PropertyType*>&);

  std::map<const std::string, const PropertyType*> _propTypeMap;
};

class Component::Props {
 public:
  Props() {}
  virtual ~Props() {}

  // Returns the shared static PropertyTypes for this Prop object.
  virtual auto getPropertyTypes() const -> const PropertyTypes*;

  // Compares the contents of this prop object against another prop object
  auto equals(const Props* oldProps) -> bool;
  auto compare(const Props* oldProps) -> std::optional<std::string>;

  bool hasProperty(const std::string& key) const { return this->getPropertyTypes()->hasProp(key); }

  // Sets one property on this prop object
  template <class T>
  void setProperty(const std::string& key, const T& value);

  // Set a property from a JSON value
  void setPropertyFromJson(const std::string& key, const Json::Value& jsonValue, const JSONConverter*);

  // Gets the general property type object for a key
  auto getPropertyType(const std::string& key) const -> const PropertyType*;

  // Get the typed property type object for a key
  template <class T>
  auto getPropertyTypeT(const std::string& key) const -> const PropertyTypeT<T>*;

 private:
  std::unique_ptr<PropertyTypes> _propTypes;
};

template <class T>
void Component::Props::setProperty(const std::string& key, const T& value) {
  std::cout << "setProperty: getPropertyTypes" << std::endl;
  auto propTypes = this->getPropertyTypes();
  if (!propTypes) {
    throw std::logic_error("Props: No prop types found");
  }
  std::cout << "setProperty: getPropertyType" << std::endl;
  if (auto propType = this->getPropertyTypes()->getPropertyType(key)) {
    std::cout << "setProperty: dynamic_cast" << std::endl;
    if (auto propTypeT = dynamic_cast<const PropertyTypeT<T>*>(propType)) {
      std::cout << "setting prop" << std::endl;
      propTypeT->set(this, value);
    }
  }
}

template <class T>
auto Component::Props::getPropertyTypeT(const std::string& key) const -> const PropertyTypeT<T>* {
  auto propertyType = this->getPropertyType(key);
  auto propTypeT = dynamic_cast<const PropertyTypeT<T>*>(propertyType);
  if (!propTypeT) {
    throw std::runtime_error("property type mismatch for " + key);
  }
  return propTypeT;
}

}  // namespace deckgl

#endif  // DECKGL_JSON_COMPONENT_H