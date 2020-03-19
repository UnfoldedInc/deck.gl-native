#include "./layers.h"

using namespace deckgl;

void deckgl::registerJSONConvertersForDeckLayers(JSONConverter *jsonConverter) {
  jsonConverter->classes["LineLayer"] = [](const Json::Value &) { return std::make_shared<LineLayer::Props>(); };

  jsonConverter->classes["ScatterplotLayer"] = [](const Json::Value &) {
    return std::make_shared<ScatterplotLayer::Props>();
  };
}
