#ifndef DECKGL_CORE_LAYER_STATE_H
#define DECKGL_CORE_LAYER_STATE_H

namespace deckgl {

class Layer;
class AttributeManager;

class LayerState {  // }: public ComponentState {
 public:
  Layer* layer;
  AttributeManager* attributeManager;
  bool needsRedraw;

  LayerState(Layer* layer_, AttributeManager* attributeManager_) {
    this->layer = layer_;
    this->attributeManager = attributeManager_;
    this->needsRedraw = true;
    // this->model = null;
    // this->subLayers = null; // reference to sublayers rendered in a
    // previous cycle
  }

  get layer() { return this->layer; }

  set layer(layer) { this->layer = layer; }
}

}  // namespace deckgl

#endif  // DECKGL_CORE_LAYER_STATE_H
