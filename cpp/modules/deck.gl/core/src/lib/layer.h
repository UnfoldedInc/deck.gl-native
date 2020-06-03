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

#ifndef DECKGL_CORE_LAYER_H
#define DECKGL_CORE_LAYER_H

#include <exception>
#include <functional>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <optional>
#include <string>

#include "./component.h"
#include "./constants.h"
#include "./layer-context.h"
#include "attribute/attribute-manager.h"
#include "deck.gl/json.h"
#include "luma.gl/core.h"
#include "luma.gl/webgpu.h"
#include "math.gl/core.h"

namespace deckgl {

// TODO(ib@unfolded.ai): This should be imported from other file
class ColorRGBA {
 public:
  ColorRGBA(float r_, float g_, float b_, float a_) : r{r_}, g{g_}, b{b_}, a{a_} {}
  float r, g, b, a;
};

class Layer : public Component {
 public:
  class Props;
  class ChangeFlags;

  const auto props() const { return std::dynamic_pointer_cast<const Layer::Props>(this->_props); }

  explicit Layer(std::shared_ptr<Layer::Props> props) : Component{std::dynamic_pointer_cast<Component::Props>(props)} {}

  /// \brief Update all props.
  void setProps(std::shared_ptr<Layer::Props> newProps);

  void triggerUpdate(const std::string& attributeName);

  /// \brief Sets the redraw flag for this layer, will trigger a redraw next animation frame.
  void setNeedsRedraw(const std::string& reason);

  /// \brief This layer needs a deep update.
  void setNeedsUpdate(const std::string& reason);

  /// \brief Checks state of attributes and model.
  auto getNeedsRedraw(bool clearRedrawFlags = false) -> std::optional<std::string>;

  /// \brief Checks if layer attributes needs updating.
  auto getNeedsUpdate() -> std::optional<std::string>;

  /* CHANGE MANAGEMENT */

  void setDataChangedFlag(const std::string& reason);
  void setPropsChangedFlag(const std::string& reason);
  void setViewportChangedFlag(const std::string& reason);
  auto getChangeFlags() -> ChangeFlags;
  void clearChangeFlags();
  void _updateChangeFlags();

  auto attributeManager() -> std::shared_ptr<AttributeManager> { return this->_attributeManager; };

  /// \brief Return an array of models used by this layer, can be overriden by layer.
  auto models() -> std::list<std::shared_ptr<lumagl::Model>> { return this->_models; };

  /// \brief Called once to set up the initial state: App can create WebGPU resources.
  virtual void initializeState();

  /// \brief Check if update cycle should run. Default returns changeFlags.propsOrDataChanged.
  virtual auto shouldUpdateState(const ChangeFlags& changeFlags, const std::shared_ptr<Layer::Props>& oldProps)
      -> const std::optional<std::string>&;

  /// \brief Default implementation: all attributes will be invalidated and updated when data changes.
  virtual void updateState(const ChangeFlags& changeFlags, const std::shared_ptr<Layer::Props>& oldProps);

  /// \brief Called once when layer is no longer matched and state will be discarded.
  /// App can destroy WebGPU resources here.
  virtual void finalizeState();

  /// \brief If state has a model, draw it with supplied uniforms
  virtual void drawState(wgpu::RenderPassEncoder pass);

  void draw(wgpu::RenderPassEncoder pass);

  const std::shared_ptr<Layer::Props> oldProps;
  std::shared_ptr<LayerContext> context;

  std::string needsRedraw;
  std::string needsUpdate;

 protected:
  /// \brief Default implementation of attribute invalidation, can be redefined.
  void _invalidateAttribute(const std::string& name = "all", const std::string& diffReason = "");

  /// \brief Calls attribute manager to update any WebGPU attributes.
  void _updateAttributes();

  std::shared_ptr<AttributeManager> _attributeManager;
  std::list<std::shared_ptr<lumagl::Model>> _models;

 private:
  // LAYER MANAGER API (Should only be called by the deck.gl LayerManager class)
  friend class LayerManager;

  /// \brief Called by layer manager when a new layer is found.
  void initialize(const std::shared_ptr<LayerContext>& context);

  /// \brief If this layer is new (not matched with an existing layer) oldProps will be empty object.
  void update();

  /// \brief Called by manager when layer is about to be disposed.
  /// \note Not guaranteed to be called on application shutdown.
  void finalize();

  // Helpers
  void _updateState();

 public:
  // PropType diffing results
  class ChangeFlags {
   public:
    // Primary changeFlags, can be strings stating reason for change
    std::optional<std::string> dataChanged;
    std::optional<std::string> propsChanged;
    std::optional<std::string> viewportChanged;
    std::optional<std::string> extensionsChanged;

    // Derived changeFlags
    std::optional<std::string> propsOrDataChanged;
    std::optional<std::string> somethingChanged;

    ChangeFlags()
        : dataChanged{std::nullopt},
          propsChanged{std::nullopt},
          viewportChanged{std::nullopt},

          // Derived changeFlags
          propsOrDataChanged{std::nullopt},
          somethingChanged{std::nullopt} {}
  };

 private:
  ChangeFlags _changeFlags;
};

class Layer::Props : public Component::Props {
 public:
  using super = Component::Props;

  std::shared_ptr<arrow::Table> data;

  bool visible{true};
  float opacity{1.0};

  COORDINATE_SYSTEM coordinateSystem{COORDINATE_SYSTEM::DEFAULT};
  mathgl::Vector3<double> coordinateOrigin;
  mathgl::Matrix4<double> modelMatrix;
  bool wrapLongitude{false};

  std::string positionFormat{"XYZ"};
  std::string colorFormat{"RGBA"};

  // Property Type Machinery
  static constexpr const char* getTypeName() { return "Layer"; }
  auto getProperties() const -> const std::shared_ptr<Properties> override;
  auto makeComponent(std::shared_ptr<Component::Props> props) const -> std::shared_ptr<Component> override {
    return std::make_shared<Layer>(std::dynamic_pointer_cast<Layer::Props>(props));
  }
};

}  // namespace deckgl

#endif  // DECKGL_CORE_LAYER_H
