#ifndef DECKGL_CORE_COMPONENT_H
#define DECKGL_CORE_COMPONENT_H

#include <functional>

#include "./prop-types.h"

namespace deckgl {

class Component {
 public:
  using Props = deckgl::Props;
  virtual ~Component() {}
};

}  // namespace deckgl

#endif  // DECKGL_CORE_COMPONENT_H
