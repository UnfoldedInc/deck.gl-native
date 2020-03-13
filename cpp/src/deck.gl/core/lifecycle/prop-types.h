//

#ifndef DECKGL_CORE_PROP_TYPES_H
#define DECKGL_CORE_PROP_TYPES_H

#include <functional>
#include <map>
#include <memory>
#include <string>

namespace deckgl {

class Props;

class Prop {
 public:
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

class Props {
 public:
  Props() {}
  virtual ~Props() {}

  auto getPropTypes() -> const std::map<const std::string, const Prop*>*;
  auto compare(const Props* oldProps) -> bool;

 protected:
  virtual auto getParentProps() const -> std::shared_ptr<Props> { return nullptr; }
  virtual auto getOwnPropTypes() const -> const std::map<const std::string, const Prop*>* = 0;

 private:
  std::map<const std::string, const Prop*> _mergedPropTypes;
};

}  // namespace deckgl

#endif  // DECKGL_CORE_PROP_TYPES_H
