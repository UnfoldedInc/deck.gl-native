#include "./prop-types.h"  // {Props}

using namespace deckgl;

// PropTypes

PropTypes::PropTypes(const std::string& className_, const PropTypes* parentPropTypes,
                     const std::map<const std::string, const PropType*>& ownPropTypeMap)
    : className{className_} {
  // TODO clang does not appear to have C++17 `map.merge()`, using `map.insert()` instead...

  // insert our prop types
  this->propTypeMap.insert(ownPropTypeMap.begin(), ownPropTypeMap.end());

  // Insert parent's prop types
  if (parentPropTypes) {
    const auto& parentPropTypeMap = parentPropTypes->propTypeMap;
    this->propTypeMap.insert(parentPropTypeMap.begin(), parentPropTypeMap.end());
  }
}

// Props

auto Props::getPropTypes() const -> const PropTypes* {
  static PropTypes propTypes{"Component", nullptr, std::map<const std::string, const PropType*>{}};
  return &propTypes;
}

auto Props::compare(const Props* oldProps) -> bool {
  auto propTypes = this->getPropTypes();

  for (auto element : propTypes->propTypeMap) {
    // Accessing KEY from element
    std::string name = element.first;
    // Accessing VALUE from element.
    const PropType* propType = element.second;
    // std::cout<<word<<" :: "<<count<<std::endl;
    if (!propType->equals(this, oldProps)) {
      return false;
    }
  }

  return true;
}

// void Props::setProperty(const std::string& key, double value) {
//   auto propTypes = this->getPropTypes();
//   if (!propTypes->hasProp(key)) {
//     throw std::runtime_error("no such property " + key);
//     // TODO - warn?
//     // return;
//   }
//   // auto propType = propTypes->getPropType(key);
//   // propType->set(this, value);
// }
