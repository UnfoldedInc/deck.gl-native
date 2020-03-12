#include "./prop-types.h"  // {Props}

using namespace deckgl;

auto Props::compare(const Props* oldProps) -> bool {
  auto propTypes = this->getPropTypes();

  for (auto element : propTypes) {
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
