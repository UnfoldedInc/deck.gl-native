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

auto AttributeManager::getNeedsRedraw(bool clearRedrawFlags) -> bool {
  bool redraw = this->_needsRedraw;
  this->_needsRedraw = this->_needsRedraw && !clearRedrawFlags;
  return redraw;
}

void AttributeManager::setNeedsRedraw() { this->_needsRedraw = true; }

void AttributeManager::add(const std::shared_ptr<AttributeDescriptor>& descriptor) { this->_add(descriptor); }

void AttributeManager::initialize() {
  // TODO(ilija): Not needed as we're creating a new table when updating?
  // Create a new table based on previously added descriptors
  std::vector<std::shared_ptr<arrow::Field>> fields = {};
  for (auto descriptor : this->_descriptors) {
    auto field = arrow::field(descriptor->name, descriptor->type);
    fields.push_back(field);
  }
  auto schema = std::make_shared<arrow::Schema>(fields);

  std::vector<std::shared_ptr<arrow::Array>> arrays{};
  this->_attributeTable = arrow::Table::Make(schema, arrays);
}

void AttributeManager::invalidate(const std::string& attributeName) { this->invalidateAll(); }

void AttributeManager::invalidateAll() {
  probegl::DebugLog() << "AttributeManager: invalidating all attributes";
  // TODO(ilija@unfolded.ai): This should trigger redraw?
}

auto AttributeManager::update(const std::shared_ptr<arrow::Table>& table) -> std::shared_ptr<arrow::Table> {
  // initialize not called
  if (!this->_attributeTable) {
    probegl::WarningLog() << "AttributeManager: Update failed, initialize not called";
    return nullptr;
  }

  // Create an empty output table
  std::vector<std::shared_ptr<arrow::Field>> fields{};
  std::vector<std::shared_ptr<arrow::Array>> arrays{};

  // Iterate over descriptors and create one column per-descriptor
  for (auto descriptor : this->_descriptors) {
    auto field = std::make_shared<arrow::Field>(descriptor->name, descriptor->type);
    fields.push_back(field);

    auto columnData = descriptor->accessor(table);
    arrays.push_back(columnData);
  }

  auto schema = std::make_shared<arrow::Schema>(fields);
  auto processedTable = arrow::Table::Make(schema, arrays);

  return processedTable;
}

void AttributeManager::_add(const std::shared_ptr<AttributeDescriptor>& descriptor, bool isInstanced) {
  this->_descriptors.push_back(descriptor);
}
