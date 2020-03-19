#include "./core.h"

using namespace deckgl;

void deckgl::registerJSONConvertersForDeckCore(JSONConverter *jsonConverter) {
  jsonConverter->classes["Deck"] = [](const Json::Value &) -> std::shared_ptr<Component::Props> {
    return std::make_shared<Deck::Props>();
  };

  jsonConverter->classes["ViewState"] = [](const Json::Value &) -> std::shared_ptr<Component::Props> {
    return std::make_shared<ViewState::Props>();
  };
}
