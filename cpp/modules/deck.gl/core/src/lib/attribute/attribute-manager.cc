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

#include <iostream>

using namespace deckgl;

auto AttributeManager::getNeedsRedraw(bool clearRedrawFlags) -> bool {
  bool redraw = this->_needsRedraw;
  this->_needsRedraw = this->_needsRedraw && !clearRedrawFlags;
  return redraw;
}

void AttributeManager::setNeedsRedraw() { this->_needsRedraw = true; }

void AttributeManager::add(const std::shared_ptr<AttributeDescriptor>& descriptor) { this->_add(descriptor); }

void AttributeManager::addInstanced(const std::shared_ptr<AttributeDescriptor>& descriptor) {
  this->_add(descriptor, true);
}

void AttributeManager::initialize() {
  // Create a new table based on previously added descriptors
  std::vector<std::shared_ptr<arrow::Field>> fields = {};
  for (auto descriptor : this->_descriptors) {
    auto field = arrow::field(descriptor->name, descriptor->type);
    fields.push_back(field);
  }
  auto schema = std::make_shared<arrow::Schema>(fields);

  std::vector<std::shared_ptr<arrow::Array>> arrays = {};
  this->_attributeTable = arrow::Table::Make(schema, arrays);

  // Reset the descriptor data
  this->_descriptors.clear();
}

void AttributeManager::invalidate(const std::string& attributeName) { this->invalidateAll(); }

void AttributeManager::invalidateAll() {
  std::cout << "AttributeManager: invalidating all attributes" << std::endl;
  // TODO(ilija): This should trigger redraw?
}

void AttributeManager::update(const std::shared_ptr<arrow::Table>& table) {
  // initialize not called
  if (!this->_attributeTable) {
    return;
  }

  // TODO(ilija): Call accessor with appropriate row data
  // TODO(ilija): Set attribute table data to accessor result
}

void AttributeManager::_add(const std::shared_ptr<AttributeDescriptor>& descriptor, bool isInstanced) {
  this->_descriptors.push_back(descriptor);
}
