#include "attribute-manager.h"

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
  // TODO: This should trigger redraw?
}

void AttributeManager::update(const std::shared_ptr<arrow::Table>& table) {
  // initialize not called
  if (!this->_attributeTable) {
    return;
  }

  // TODO: Call accessor with appropriate row data
  // TODO: Set attribute table data to accessor result
}

void AttributeManager::_add(const std::shared_ptr<AttributeDescriptor>& descriptor, bool isInstanced) {
  this->_descriptors.push_back(descriptor);
}
