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

#include "./layer.h"  // NOLINT(build/include)

#include "./layer-manager.h"

using namespace mathgl;
using namespace deckgl;

// Setters and getters for properties
// TODO(ib@unfolded.ai): auto generate from language-independent prop definition schema
static const std::vector<std::shared_ptr<Property>> propTypeDefs = {
    std::make_shared<PropertyT<bool>>(
        "visible", [](const JSONObject* props) { return dynamic_cast<const Layer::Props*>(props)->visible; },
        [](JSONObject* props, bool value) { return dynamic_cast<Layer::Props*>(props)->visible = value; }, true),
    std::make_shared<PropertyT<float>>(
        "opacity", [](const JSONObject* props) { return dynamic_cast<const Layer::Props*>(props)->opacity; },
        [](JSONObject* props, float value) { return dynamic_cast<Layer::Props*>(props)->opacity = value; }, 1.0),
    std::make_shared<PropertyT<COORDINATE_SYSTEM>>(
        "coordinateSystem",
        [](const JSONObject* props) { return dynamic_cast<const Layer::Props*>(props)->coordinateSystem; },
        [](JSONObject* props, COORDINATE_SYSTEM value) {
          return dynamic_cast<Layer::Props*>(props)->coordinateSystem = value;
        },
        COORDINATE_SYSTEM::DEFAULT),
    std::make_shared<PropertyT<Vector3<double>>>(
        "coordinateOrigin",
        [](const JSONObject* props) { return dynamic_cast<const Layer::Props*>(props)->coordinateOrigin; },
        [](JSONObject* props, Vector3<double> value) {
          return dynamic_cast<Layer::Props*>(props)->coordinateOrigin = value;
        },
        Vector3<double>()),
    std::make_shared<PropertyT<Matrix4<double>>>(
        "modelMatrix", [](const JSONObject* props) { return dynamic_cast<const Layer::Props*>(props)->modelMatrix; },
        [](JSONObject* props, Matrix4<double> value) {
          return dynamic_cast<Layer::Props*>(props)->modelMatrix = value;
        },
        Matrix4<double>()),
    std::make_shared<PropertyT<bool>>(
        "wrapLongitude",
        [](const JSONObject* props) { return dynamic_cast<const Layer::Props*>(props)->wrapLongitude; },
        [](JSONObject* props, bool value) { return dynamic_cast<Layer::Props*>(props)->wrapLongitude = value; }, false),
    std::make_shared<PropertyT<std::string>>(
        "positionFormat",
        [](const JSONObject* props) { return dynamic_cast<const Layer::Props*>(props)->positionFormat; },
        [](JSONObject* props, std::string value) { return dynamic_cast<Layer::Props*>(props)->positionFormat = value; },
        "XYZ"),
    std::make_shared<PropertyT<std::string>>(
        "colorFormat", [](const JSONObject* props) { return dynamic_cast<const Layer::Props*>(props)->colorFormat; },
        [](JSONObject* props, std::string value) { return dynamic_cast<Layer::Props*>(props)->colorFormat = value; },
        "RGBA")};

auto Layer::Props::getProperties() const -> const std::shared_ptr<Properties> {
  static auto properties = Properties::from<Layer::Props>("Layer", propTypeDefs);
  return properties;
}

void Layer::setProps(std::shared_ptr<Layer::Props> newProps) {
  this->_props = newProps;
  this->setNeedsUpdate("Props updated");
  this->setNeedsRedraw("Props updated");
}

void Layer::triggerUpdate(const std::string& gpuColumnName) {
  this->_attributeManager->invalidate(gpuColumnName);
  this->setNeedsUpdate(gpuColumnName);
}

void Layer::setNeedsRedraw(const std::string& reason) { this->needsRedraw = reason; }

void Layer::setNeedsUpdate(const std::string& reason) {
  this->context->layerManager->setNeedsUpdate(reason);
  this->needsUpdate = reason;
}

auto Layer::getNeedsRedraw(bool clearRedrawFlags) -> std::optional<std::string> {
  return "TODO: Layers always need to be redrawn";  // this->_getNeedsRedraw(clearRedrawFlags);
}

auto Layer::getNeedsUpdate() -> std::optional<std::string> {
  return "Not implemented";
  //   // Call subclass lifecycle method
  //   return (
  //     this->internalState->needsUpdate ||
  //     this->hasUniformTransition() ||
  //     this->shouldUpdateState(this->_getUpdateParams())
  //   );
  //   // End lifecycle method
}

void Layer::initializeState() {}

auto Layer::shouldUpdateState(const Layer::ChangeFlags& changeFlags, const std::shared_ptr<Layer::Props>& oldProps)
    -> const std::optional<std::string>& {
  return changeFlags.propsOrDataChanged;
}

void Layer::updateState(const Layer::ChangeFlags& changeFlags, const std::shared_ptr<Layer::Props>& oldProps) {
  /*
  const auto attributeManager = this->getAttributeManager();
  if (changeFlags.dataChanged && attributeManager) {
    const dataChanged = changeFlags.dataChanged;
    if (Array.isArray(dataChanged)) {
      // is partial update
      for (const dataRange of dataChanged) {
        attributeManager->invalidateAll(dataRange);
      }
    } else {
      attributeManager->invalidateAll();
    }
  }
  */
}

void Layer::finalizeState() {}

void Layer::drawState(wgpu::RenderPassEncoder pass) {
  for (auto model : this->models()) {
    model->draw(pass);
  }
}

void Layer::draw(wgpu::RenderPassEncoder pass) {
  // Call subclass lifecycle method
  this->drawState(pass);
  // End lifecycle method
}

void Layer::_invalidateAttribute(const std::string& name, const std::string& diffReason) {
  if (name == "all") {
    this->_attributeManager->invalidateAll();
  } else {
    this->_attributeManager->invalidate(name);
  }
}

void Layer::_updateAttributes() {
  /*
    const auto attributeManager = this->getAttributeManager();
    if (!attributeManager) {
      return;
    }

    // Figure out data length
    auto numInstances = this->getNumInstances(props);
    auto startIndices = 0; this->getStartIndices(props);

    attributeManager->update({
      data: props.data,
      numInstances,
      startIndices,
      transitions: props.transitions,
      buffers: props.data.attributes,
      context: this,
      // Don"t worry about non-attribute props
      ignoreUnknownAttributes: true
    });

  // const auto changedAttributes =
  // attributeManager->getChangedAttributes({clearChangedFlags: true});
  // this->updateAttributes(changedAttributes);
  */
}

void Layer::initialize(const std::shared_ptr<LayerContext>& context) {
  this->context = context;
  this->_attributeManager = std::make_shared<AttributeManager>(this->props()->id, context->device);

  // Call subclass lifecycle method
  this->initializeState();
  // End subclass lifecycle method

  auto reason = "Layer initialization";
  this->setDataChangedFlag(reason);
  this->setPropsChangedFlag(reason);
  this->setViewportChangedFlag(reason);

  this->_updateState();
}

void Layer::update() {
  // Call subclass lifecycle method
  auto stateNeedsUpdate = this->getNeedsUpdate();
  // End lifecycle method

  if (stateNeedsUpdate) {
    this->_updateState();
  }
}

// Common code for _initialize and _update
void Layer::_updateState() {
  // Safely call subclass lifecycle methods
  // if (!this->context->gl) {
  //   return;
  // }

  this->updateState(this->_changeFlags, this->oldProps);
  // End subclass lifecycle methods

  // if (this->isComposite) {
  //   // Render or update previously rendered sublayers
  //   this->_renderLayers();
  // } else {

  this->setNeedsRedraw("Update state");
  // Add any subclass attributes
  this->_updateAttributes();

  // this->props = currentProps;
  // this->oldProps = nullptr;
  this->clearChangeFlags();
  this->needsUpdate = false;
  this->needsRedraw = true;
}

// Called by manager when layer is about to be disposed
// Note: not guaranteed to be called on application shutdown
void Layer::finalize() {
  // debug(TRACE_FINALIZE, this);

  // Call subclass lifecycle method
  this->finalizeState();
  // End subclass lifecycle method
}

auto Layer::getChangeFlags() -> Layer::ChangeFlags { return this->_changeFlags; }

void Layer::setDataChangedFlag(const std::string& reason) {
  if (!this->_changeFlags.dataChanged) {
    this->_changeFlags.dataChanged = reason;
  }
  this->_updateChangeFlags();
}

void Layer::setPropsChangedFlag(const std::string& reason) {
  if (!this->_changeFlags.propsChanged) {
    this->_changeFlags.propsChanged = reason;
  }
  this->_updateChangeFlags();
}

void Layer::setViewportChangedFlag(const std::string& reason) {
  if (!this->_changeFlags.viewportChanged) {
    this->_changeFlags.viewportChanged = reason;
  }
  this->_updateChangeFlags();
}

void Layer::clearChangeFlags() {
  // Primary changeFlags, can be strings stating reason for change
  this->_changeFlags.dataChanged = std::nullopt;
  this->_changeFlags.propsChanged = std::nullopt;
  this->_changeFlags.viewportChanged = std::nullopt;

  this->_changeFlags.propsOrDataChanged = std::nullopt;
  this->_changeFlags.somethingChanged = std::nullopt;
}

void Layer::_updateChangeFlags() {
  // Update composite flags
  if (!this->_changeFlags.propsOrDataChanged) {
    if (this->_changeFlags.dataChanged) {
      this->_changeFlags.propsOrDataChanged = this->_changeFlags.dataChanged;
    } else if (this->_changeFlags.propsChanged) {
      this->_changeFlags.propsOrDataChanged = this->_changeFlags.propsChanged;
    }
  }

  if (!this->_changeFlags.somethingChanged) {
    if (this->_changeFlags.propsOrDataChanged) {
      this->_changeFlags.somethingChanged = this->_changeFlags.propsOrDataChanged;
    } else if (this->_changeFlags.viewportChanged) {
      this->_changeFlags.somethingChanged = this->_changeFlags.viewportChanged;
    }
  }
}
