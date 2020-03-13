#include "./prop-types.h"  // {Props}

using namespace deckgl;

//
PropTypes::PropTypes(const PropTypes* parentPropTypes, const std::map<const std::string, const Prop*>& ownPropTypeMap) {
  // TODO clang does not appear to have C++17 `map.merge()`, using `map.insert()` instead...

  // insert our prop types
  this->propTypeMap.insert(ownPropTypeMap.begin(), ownPropTypeMap.end());

  // Insert parent's prop types
  if (parentPropTypes) {
    const auto& parentPropTypeMap = parentPropTypes->propTypeMap;
    this->propTypeMap.insert(parentPropTypeMap.begin(), parentPropTypeMap.end());
  }
}

auto Props::compare(const Props* oldProps) -> bool {
  auto propTypes = this->getPropTypes();

  for (auto element : propTypes->propTypeMap) {
    // Accessing KEY from element
    std::string name = element.first;
    // Accessing VALUE from element.
    const Prop* propType = element.second;
    // std::cout<<word<<" :: "<<count<<std::endl;
    if (!propType->equals(this, oldProps)) {
      return false;
    }
  }

  return true;
}
