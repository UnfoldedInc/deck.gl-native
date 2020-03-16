#include "./prop-types.h"  // {Props}

using namespace deckgl;

// PropertyTypes

PropertyTypes::PropertyTypes(const std::string& className_, const PropertyTypes* parentPropertyTypes,
                             const std::vector<const PropertyType*>& ownPropertyTypeDefs)
    : className{className_}, parent{parentPropertyTypes} {
  // insert our prop types
  for (auto element : ownPropertyTypeDefs) {
    this->propTypeMap.insert({element->name, element});
  }

  // Insert parent's prop types
  if (parentPropertyTypes) {
    const auto& parentPropertyTypeMap = parentPropertyTypes->propTypeMap;
    this->propTypeMap.insert(parentPropertyTypeMap.begin(), parentPropertyTypeMap.end());
  }
}

// Props

auto Props::getPropertyTypes() const -> const PropertyTypes* {
  static PropertyTypes propTypes{"Component", nullptr, std::vector<const PropertyType*>{}};
  return &propTypes;
}

void Props::setPropertyFromJson(const std::string& key, const Json::Value& jsonValue) {
  auto propertyType = this->getPropertyType(key);
  propertyType->setPropertyFromJson(this, jsonValue);
}

auto Props::getPropertyType(const std::string& key) const -> const PropertyType* {
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

auto Props::compare(const Props* oldProps) -> bool {
  auto propTypes = this->getPropertyTypes();

  for (auto element : propTypes->propTypeMap) {
    // Accessing KEY from element
    std::string name = element.first;
    // Accessing VALUE from element.
    const PropertyType* propType = element.second;
    // std::cout<<word<<" :: "<<count<<std::endl;
    if (!propType->equals(this, oldProps)) {
      return false;
    }
  }

  return true;
}
