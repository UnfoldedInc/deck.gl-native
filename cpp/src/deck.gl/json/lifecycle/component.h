#ifndef DECKGL_JSON_COMPONENT_H
#define DECKGL_JSON_COMPONENT_H

#include <functional>

namespace deckgl {

class Props;

class Component {
 public:
  using Props = Props;
  virtual ~Component() {}
};

}  // namespace deckgl

#endif  // DECKGL_JSON_COMPONENT_H
