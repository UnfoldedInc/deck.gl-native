#include "./prop-types.h"  // {Props}

using namespace deckgl;

//
// TODO clang does not appear to have C++17 map.merge(), using map.insert()
auto Props::getPropTypes() -> const std::map<const std::string, const Prop*>* {
  if (this->_mergedPropTypes.empty()) {
    const auto* ownPropTypes = this->getOwnPropTypes();
    this->_mergedPropTypes.insert(ownPropTypes->begin(), ownPropTypes->end());
    if (auto parentProps = this->getParentProps()) {
      const auto* parentPropTypes = parentProps->getOwnPropTypes();
      this->_mergedPropTypes.insert(parentPropTypes->begin(), parentPropTypes->end());
    }
  }
  return &(this->_mergedPropTypes);
}

auto Props::compare(const Props* oldProps) -> bool {
  auto propTypes = this->getPropTypes();

  for (auto element : *propTypes) {
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
