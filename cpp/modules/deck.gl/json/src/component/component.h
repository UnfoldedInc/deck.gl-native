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
  using Props = JSONObject;
  virtual ~Component() {}
};

}  // namespace deckgl

#endif  // DECKGL_JSON_COMPONENT_H
