#include "./core.h"

using namespace deckgl;

void deckgl::registerJSONConvertersForDeckCore(JSONConverter *jsonConverter) {
  jsonConverter->classes["Deck"] = [](const Json::Value &) { return std::make_shared<Deck::Props>(); };

  jsonConverter->classes["ViewState"] = [](const Json::Value &) { return std::make_shared<ViewState>(); };
}
