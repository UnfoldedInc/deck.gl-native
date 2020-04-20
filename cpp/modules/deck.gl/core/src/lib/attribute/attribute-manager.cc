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

#include "./attribute-manager.h"  // NOLINT(build/include)

#include "probe.gl/core.h"

using namespace deckgl;
using namespace lumagl;

auto AttributeManager::getNeedsRedraw(bool clearRedrawFlags) -> bool {
  bool redraw = this->_needsRedraw;
  this->_needsRedraw = this->_needsRedraw && !clearRedrawFlags;
  return redraw;
}

void AttributeManager::setNeedsRedraw() { this->_needsRedraw = true; }

void AttributeManager::add(const lumagl::garrow::AttributeDescriptor& descriptor) {
  this->_descriptors.push_back(std::move(descriptor));
}

void AttributeManager::invalidate(const std::string& attributeName) {
  // TODO(ilija@unfolded.ai): Implement
  this->invalidateAll();
}

void AttributeManager::invalidateAll() {
  // TODO(ilija@unfolded.ai): Implement
  probegl::DebugLog() << "AttributeManager: invalidating all attributes";
}

auto AttributeManager::update(const std::shared_ptr<arrow::Table>& table) -> std::shared_ptr<garrow::Table> {
  return garrow::transformTable(table, this->_descriptors, this->device);
}
