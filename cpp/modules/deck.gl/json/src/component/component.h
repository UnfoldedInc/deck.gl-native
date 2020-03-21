#ifndef DECKGL_JSON_COMPONENT_H
#define DECKGL_JSON_COMPONENT_H

#include <functional>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <string>

#include "../converter/json-types-mathgl.h"  // {fromJson<T>(math.gl types)}
#include "../converter/json-types.h"         // {fromJson<T>(std types)}
#include "../json-object/json-object.h"      // {JSONObject}
#include "json/json.h"                       // {Json::Value}

namespace deckgl {

// Forward declarations from other files
class JSONConverter;  // #include "../converter/json-converter.h"

class Component {
 public:
  class Props;

  explicit Component(std::shared_ptr<Props> props) : _props{props} {}
  virtual ~Component() {}

 protected:
  std::shared_ptr<Props> _props;
};

class Component::Props : public JSONObject {
 public:
  using super = JSONObject;
  // virtual auto createComponent() const -> std::shared_ptr<Component> = 0;
};

}  // namespace deckgl

#endif  // DECKGL_JSON_COMPONENT_H
