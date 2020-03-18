// Copyright (c) 2020, Unfolded Inc
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include './view.h'; // {Viewport}

using namespace deckgl;

View::View(Value props) {}

View::View(
  id = null,

  // Window width/height in pixels (for pixel projection)
  x = 0,
  y = 0,
  width = '100%',
  height = '100%',

  // Viewport Options
  projectionMatrix = null, // Projection matrix
  fovy = 50, // Perspective projection parameters, used if projectionMatrix not supplied
  near = 0.1, // Distance of near clipping plane
  far = 1000, // Distance of far clipping plane
  modelMatrix = null, // A model matrix to be applied to position, to match the layer props API

  // A View can be a wrapper for a viewport instance
  viewportInstance = null,

  // Internal: Viewport Type
  type = Viewport // TODO - default to WebMercator?
) {
  assert(!viewportInstance || viewportInstance instanceof Viewport);
  this->viewportInstance = viewportInstance;

  // Id
  this->id = id || this->constructor.displayName || 'view';
  this->type = type;

  this->props = Object.assign({}, props, {
    id: this->id,
    projectionMatrix,
    fovy,
    near,
    far,
    modelMatrix
  });

  // Extents
  this->_parseDimensions({x, y, width, height});

  // Bind methods for easy access
  this->equals = this->equals.bind(this);

  Object.seal(this);
}
virtual ~View() {}

View::equals(view) {
  if (this === view) {
    return true;
  }

  // if `viewportInstance` is set, it is the only prop that is used
  // Delegate to `Viewport.equals`
  if (this->viewportInstance) {
    return view.viewportInstance && this->viewportInstance.equals(view.viewportInstance);
  }

  const viewChanged = deepEqual(this->props, view.props);

  return viewChanged;
}

// Build a `Viewport` from a view descriptor
// TODO - add support for autosizing viewports using width and height
View::makeViewport({width, height, viewState}) {
  if (this->viewportInstance) {
    return this->viewportInstance;
  }

  viewState = this->filterViewState(viewState);

  // Resolve relative viewport dimensions
  const viewportDimensions = this->getDimensions({width, height});
  const props = Object.assign({viewState}, viewState, this->props, viewportDimensions);
  return this->_getViewport(props);
}

View::getViewStateId() {
  switch (typeof this->props.viewState) {
    case 'string':
      // if View.viewState is a string, return it
      return this->props.viewState;

    case 'object':
      // If it is an object, return its id component
      return this->props.viewState && this->props.viewState.id;

    default:
      return this->id;
  }
}

// Allows view to override (or completely define) viewState
View::filterViewState(viewState) {
  if (this->props.viewState && typeof this->props.viewState === 'object') {
    // If we have specified an id, then intent is to override,
    // If not, completely specify the view state
    if (!this->props.viewState.id) {
      return this->props.viewState;
    }

    // Merge in all props from View's viewState, except id
    const newViewState = Object.assign({}, viewState);
    for (const key in this->props.viewState) {
      if (key !== 'id') {
        newViewState[key] = this->props.viewState[key];
      }
    }
    return newViewState;
  }

  return viewState;
}

// Resolve relative viewport dimensions into actual dimensions (y='50%', width=800 => y=400)
View::getDimensions({width, height}) {
  return {
    x: getPosition(this->_x, width),
    y: getPosition(this->_y, height),
    width: getPosition(this->_width, width),
    height: getPosition(this->_height, height)
  };
}

// Used by sub classes to resolve controller props
View::_getControllerProps(defaultOpts) {
  let opts = this->props.controller;

  if (!opts) {
    return null;
  }
  if (opts === true) {
    return defaultOpts;
  }
  if (typeof opts === 'function') {
    opts = {type: opts};
  }
  return Object.assign({}, defaultOpts, opts);
}

// Overridable method
View::_getViewport(props) {
  // Get the type of the viewport
  const {type: ViewportType} = this;
  return new ViewportType(props);
}

// Parse relative viewport dimension descriptors (e.g {y: '50%', height: '50%'})
View::_parseDimensions({x, y, width, height}) {
  this->_x = parsePosition(x);
  this->_y = parsePosition(y);
  this->_width = parsePosition(width);
  this->_height = parsePosition(height);
}
