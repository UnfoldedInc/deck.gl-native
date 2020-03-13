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
  virtual ~Prop() {}
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

class PropTypes {
  friend class Props;

 public:
  // static methods
  template <typename ComponentT>
  static auto from(const std::map<const std::string, const Prop*>& propTypeMap) -> deckgl::PropTypes {
    typename ComponentT::super::Props parentProps;
    return PropTypes{parentProps.getPropTypes(), propTypeMap};
  }

  // public members
  // TODO - make private / iterable
  std::map<const std::string, const Prop*> propTypeMap;

  // methods
  PropTypes(const PropTypes* parentProps, const std::map<const std::string, const Prop*>&);

  bool hasProperty(const std::string& key) const { return this->propTypeMap.count(key) == 1; }
  // getProperty(const std::string &prop) const { return this->propTypeMap.count() == 1; }
  // setProperty(const std::string &prop) const { return this->propTypeMap.count() == 1; }
};

class Props {
 public:
  Props() {}
  virtual ~Props() {}

  virtual auto getPropTypes() const -> const PropTypes* {
    return new PropTypes(nullptr, std::map<const std::string, const Prop*>{});
  }
  auto compare(const Props* oldProps) -> bool;

 private:
  std::unique_ptr<PropTypes> _propTypes;
};

}  // namespace deckgl

#endif  // DECKGL_CORE_PROP_TYPES_H
