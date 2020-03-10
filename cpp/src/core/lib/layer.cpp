#include "layer.h"

/*
template <class Layer> struct PropType<Layer, bool> : public Prop {
public:
  bool defaultValue;
  std::function<auto (Layer*) -> bool> get;
  std::function<void (Layer*, bool)> set;

  PropType<Layer>(
    bool defaultValue_,
    const std::function<auto (Layer*) -> bool>& get_,
    const std::function<void (Layer*, bool)>& set_
  )
    : defaultValue{defaultValue_}
    , get{get_}
    , set{set_}
  {}
};
*/

// CLASS LayerProps
// TODO - auto generate from language-independent prop definition schema

std::map<std::string, const Prop*> LayerProps::propTypes = {
  {"visible", new PropType<Layer, bool>{
    [](Layer::Props const* props) { return props->visible; },
    [](Layer::Props* props, bool value) { return props->visible = value; },
    true
  }},
  {"pickable", new PropType<Layer, bool>{
    [](Layer::Props const* props) { return props->pickable; },
    [](Layer::Props* props, bool value) { return props->pickable = value; },
    false
  }},
  {"opacity", new PropType<Layer, float>{
    [](Layer::Props const* props) { return props->opacity; },
    [](Layer::Props* props, float value) { return props->opacity = value; },
    1.0
  }},
  {"coordinateSystem", new PropType<Layer, COORDINATE_SYSTEM>{
    [](Layer::Props const* props) { return props->coordinateSystem; },
    [](Layer::Props* props, COORDINATE_SYSTEM value) { return props->coordinateSystem = value; },
    COORDINATE_SYSTEM::DEFAULT
  }},
  {"coordinateOrigin", new PropType<Layer, Vector3<double>>{
    [](Layer::Props const* props) { return props->coordinateOrigin; },
    [](Layer::Props* props, Vector3<double> value) { return props->coordinateOrigin = value; },
    Vector3<double>()
  }},
  {"modelMatrix", new PropType<Layer, Matrix4<double>>{
    [](Layer::Props const* props) { return props->modelMatrix; },
    [](Layer::Props* props, Matrix4<double> value) { return props->modelMatrix = value; },
    Matrix4<double>()
  }},
  {"wrapLongitude", new PropType<Layer, bool>{
    [](Layer::Props const* props) { return props->wrapLongitude; },
    [](Layer::Props* props, bool value) { return props->wrapLongitude = value; },
    false
  }}
};

/*
class LayerPropTypes {
  // data: Special handling for null, see below
  // data: {type: 'data', value: EMPTY_ARRAY, async: true},
  // dataComparator: null,
  // _dataDiff: {type: 'function', value: data => data && data.__diff, compare: false, optional: true},
  // dataTransform: {type: 'function', value: null, compare: false, optional: true},
  // onDataLoad: {type: 'function', value: null, compare: false, optional: true},
  // fetch: {
  //   type: 'function',
  //   value: (url, {layer}) => load(url, layer.getLoadOptions()),
  //   compare: false
  // },
  // updateTriggers: {}, // Update triggers: a core change detection mechanism in deck.gl

  opacity: {type: 'number', min: 0, max: 1, value: 1},

  onHover: {type: 'function', value: null, compare: false, optional: true},
  onClick: {type: 'function', value: null, compare: false, optional: true},
  onDragStart: {type: 'function', value: null, compare: false, optional: true},
  onDrag: {type: 'function', value: null, compare: false, optional: true},
  onDragEnd: {type: 'function', value: null, compare: false, optional: true},

  coordinateSystem: COORDINATE_SYSTEM.DEFAULT,
  coordinateOrigin: {type: 'array', value: [0, 0, 0], compare: true},
  modelMatrix: {type: 'array', value: null, compare: true, optional: true},
  wrapLongitude: false,
  positionFormat: 'XYZ',
  colorFormat: 'RGBA',

  parameters: {},
  uniforms: {},
  extensions: [],

  // Offset depth based on layer index to avoid z-fighting.
  // Negative values pull layer towards the camera
  // https://www.opengl.org/archives/resources/faq/technical/polygonoffset.htm
  getPolygonOffset: {
    type: 'function',
    value: ({layerIndex}) => [0, -layerIndex * 100],
    compare: false
  },

  // Selection/Highlighting
  highlightedObjectIndex: null,
  autoHighlight: false,
  highlightColor: {type: 'color', value: [0, 0, 128, 128]}
};
*/

auto LayerProps::compare(const LayerProps *oldProps) -> bool {
  auto propTypes = this->getPropTypes();

  for (std::pair<std::string, const Prop *> element : propTypes) {
    // Accessing KEY from element
    std::string name = element.first;
    // Accessing VALUE from element.
    const Prop *propType = element.second;
    // std::cout<<word<<" :: "<<count<<std::endl;
    if (!propType->equals(this, oldProps)) {
      return false;
    }
  }

  return true;
}
