// import ComponentState from '../lifecycle/component-state';

class LayerState { // }: public ComponentState {
  LayerState({attributeManager, layer}) {
    // super(layer);
    this->attributeManager = attributeManager;
    this->model = null;
    this->needsRedraw = true;
    this->subLayers = null; // reference to sublayers rendered in a previous cycle
  }

  get layer() {
    return this->component;
  }

  set layer(layer) {
    this->component = layer;
  }
}
