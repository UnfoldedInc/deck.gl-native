//

#ifndef DECKGL_CORE_PROP_TYPES_H
#define DECKGL_CORE_PROP_TYPES_H

#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <string>

namespace deckgl {

class Props;

class PropType {
 public:
  virtual ~PropType() {}
  virtual bool equals(const Props*, const Props*) const = 0;
};

template <class T>
struct PropTypeT : public PropType {
 public:
  std::function<auto(Props const*)->T> get;
  std::function<void(Props*, T)> set;
  T defaultValue;

  PropTypeT<T>(const std::function<auto(Props const*)->T>& get_, const std::function<void(Props*, T)>& set_,
               const T& defaultValue_)
      : get{get_}, set{set_}, defaultValue{defaultValue_} {}

  bool equals(const Props* props1, const Props* props2) const { return this->get(props1) == this->get(props2); }
};

class PropTypes {
  friend class Props;

 public:
  // static methods
  template <typename ComponentT>
  static auto from(const std::string& className, const std::map<const std::string, const PropType*>& propTypeMap)
      -> deckgl::PropTypes {
    typename ComponentT::super::Props parentProps;
    return PropTypes{className, parentProps.getPropTypes(), propTypeMap};
  }

  // public members
  const std::string className;

  // methods
  PropTypes(const std::string& className, const PropTypes* parentProps,
            const std::map<const std::string, const PropType*>&);

  bool hasProp(const std::string& key) const { return this->propTypeMap.count(key) == 1; }
  auto getPropType(const std::string& key) const -> const PropType* { return this->propTypeMap.at(key); }

  // protected:
  // TODO - make private
  std::map<const std::string, const PropType*> propTypeMap;
};

class Props {
 public:
  Props() {}
  virtual ~Props() {}

  // Returns the shared static PropTypes for this Prop object.
  virtual auto getPropTypes() const -> const PropTypes*;

  // Compares the contents of this prop object against another prop object
  auto compare(const Props* oldProps) -> bool;

  // Sets one property on this prop object
  template <class T>
  void setProperty(const std::string& key, const T& value) {
    std::cout << "setProperty: getPropTypes" << std::endl;
    auto propTypes = this->getPropTypes();
    if (!propTypes) {
      throw std::logic_error("Props: No prop types found");
    }
    std::cout << "setProperty: getPropType" << std::endl;
    if (auto propType = this->getPropTypes()->getPropType(key)) {
      std::cout << "setProperty: dynamic_cast" << std::endl;
      if (auto propTypeT = dynamic_cast<const PropTypeT<T>*>(propType)) {
        std::cout << "setting prop" << std::endl;
        propTypeT->set(this, value);
      }
    }
  }

 private:
  std::unique_ptr<PropTypes> _propTypes;
};

}  // namespace deckgl

#endif  // DECKGL_CORE_PROP_TYPES_H
