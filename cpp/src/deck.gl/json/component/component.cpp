#include "./component.h"  // {Props}

#include "../converter/json-converter.h"  // {JSONConverter}
using namespace deckgl;

// PropertyType

auto PropertyType::_getPropListFromJson(Component::Props* props, const Json::Value& jsonValue,
                                        const JSONConverter* jsonConverter) const
    -> std::list<std::shared_ptr<Component::Props>> {
  if (jsonValue.isArray()) {
    std::list<std::shared_ptr<Component::Props>> propsList;
    for (Json::Value::ArrayIndex i = 0; i < jsonValue.size(); ++i) {
      std::shared_ptr<Component::Props> props = {jsonConverter->convertJson(jsonValue[1])};
      propsList.push_back(props);
    }
    return propsList;
  }
  throw std::runtime_error("Cannot convert JSON to list: " + this->getName());
}

// PropertyTypes

PropertyTypes::PropertyTypes(const std::string& className_, const PropertyTypes* parentPropertyTypes,
                             const std::vector<const PropertyType*>& ownPropertyTypeDefs)
    : className{className_}, parent{parentPropertyTypes} {
  // insert our prop types
  for (auto element : ownPropertyTypeDefs) {
    this->_propTypeMap.insert({element->name, element});
  }

  // Insert parent's prop types
  if (parentPropertyTypes) {
    const auto& parentPropertyTypeMap = parentPropertyTypes->_propTypeMap;
    this->_propTypeMap.insert(parentPropertyTypeMap.begin(), parentPropertyTypeMap.end());
  }
}

// Props

auto Component::Props::getPropertyTypes() const -> const PropertyTypes* {
  static PropertyTypes propTypes{"Component", nullptr, std::vector<const PropertyType*>{}};
  return &propTypes;
}

void Component::Props::setPropertyFromJson(const std::string& key, const Json::Value& jsonValue,
                                           const JSONConverter* jsonConverter) {
  auto propertyType = this->getPropertyType(key);
  propertyType->setPropertyFromJson(this, jsonValue, jsonConverter);
}

auto Component::Props::getPropertyType(const std::string& key) const -> const PropertyType* {
  // std::cout << "getPropertyTypes" << std::endl;
  auto propTypes = this->getPropertyTypes();
  if (!propTypes) {
    throw std::logic_error("Component does not have property types");
  }

  // std::cout << "getPropertyType" << std::endl;
  auto propertyType = this->getPropertyTypes()->getPropertyType(key);
  if (!propertyType) {
    throw std::runtime_error("Prop type not found for property " + key);
  }

  return propertyType;
}

auto Component::Props::equals(const Props* oldProps) -> bool {
  auto propTypes = this->getPropertyTypes();

  for (auto element : propTypes->_propTypeMap) {
    // Accessing KEY from element
    std::string name = element.first;
    // Accessing VALUE from element.
    const PropertyType* propType = element.second;
    if (!propType->equals(this, oldProps)) {
      return false;
    }
  }

  return true;
}

auto Component::Props::compare(const Props* oldProps) -> std::optional<std::string> {
  auto propTypes = this->getPropertyTypes();

  for (auto element : propTypes->_propTypeMap) {
    // Accessing KEY from element
    std::string name = element.first;
    // Accessing VALUE from element.
    const PropertyType* propType = element.second;
    if (!propType->equals(this, oldProps)) {
      return std::string{propTypes->className} + propType->name + " changed";
    }
  }

  return std::nullopt;
}
