// Copyright (c) 2020 Unfolded, Inc.
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

#ifndef DECKGL_CORE_LAYER_CONTEXT_H
#define DECKGL_CORE_LAYER_CONTEXT_H

#include <memory>

#include "deck.gl/core/src/viewports/web-mercator-viewport.h"
#include "luma.gl/webgpu.h"

namespace deckgl {

class Deck;
class LayerManager;
class Viewport;

// LayerContext is data shared between all layers
class LayerContext {
 public:
  // TODO(ilija@unfolded.ai): Do we need to have this circular dependency here?
  Deck* deck;
  wgpu::Device device;
  float devicePixelRatio;

  // TODO(ilija@unfolded.ai): Do we need to have this circular dependency here?
  LayerManager* layerManager;
  // Make sure context.viewport is not empty on the first layer initialization
  std::shared_ptr<Viewport> viewport{new WebMercatorViewport{{}}};

  // // General resources
  // stats: null, // for tracking lifecycle performance
  // // GL Resources
  // shaderCache: null,
  // pickingFBO: null, // Screen-size framebuffer that layers can reuse
  // mousePosition: null,
  // userData: {} // Place for any custom app `context`

  LayerContext(Deck* deck, wgpu::Device device, float devicePixelRatio = 1.0)
      : deck{deck}, device{device}, devicePixelRatio{devicePixelRatio} {}
};

}  // namespace deckgl

#endif  // DECKGL_CORE_LAYER_CONTEXT_H
