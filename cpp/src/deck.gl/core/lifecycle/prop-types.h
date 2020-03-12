//

#ifndef DECKGL_CORE_PROP_TYPES_H
#define DECKGL_CORE_PROP_TYPES_H

#include <functional>

namespace deckgl {

class Props {
 public:
  virtual ~Props() {}
};

struct Prop {
  virtual bool equals(const Props*, const Props*) const { return false; }
};

template <class ComponentT, class T>
struct PropType : public Prop {
 public:
  std::function<auto(typename ComponentT::Props const*)->T> get;
  std::function<void(typename ComponentT::Props*, T)> set;
  T defaultValue;

  PropType<ComponentT, T>(const std::function<auto(typename ComponentT::Props const*)->T>& get_,
                          const std::function<void(typename ComponentT::Props*, T)>& set_, T defaultValue_)
      : get{get_}, set{set_}, defaultValue{defaultValue_} {}

  bool equals(const Props* props1, const Props* props2) const {
    auto componentProps1 = dynamic_cast<const typename ComponentT::Props*>(props1);
    auto componentProps2 = dynamic_cast<const typename ComponentT::Props*>(props2);
    return this->get(componentProps1) == this->get(componentProps2);
  }
};

}  // namespace deckgl

#endif  // DECKGL_CORE_PROP_TYPES_H
