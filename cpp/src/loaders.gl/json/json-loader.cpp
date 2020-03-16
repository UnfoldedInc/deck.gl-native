#include "./json-loader.h"

#include <arrow/api.h>
#include <arrow/json/api.h>

#include <iostream>
#include <memory>

using namespace loadersgl;

auto JSONLoader::loadTable(const std::shared_ptr<arrow::io::InputStream> input) -> std::shared_ptr<arrow::Table> {
  arrow::Status status;
  arrow::MemoryPool *pool = arrow::default_memory_pool();

  auto readOptions = arrow::json::ReadOptions::Defaults();
  auto parseOptions = arrow::json::ParseOptions::Defaults();

  // Instantiate TableReader from input stream and options
  std::shared_ptr<arrow::json::TableReader> reader;
  status = arrow::json::TableReader::Make(pool, input, readOptions, parseOptions, &reader);
  if (!status.ok()) {
    // Ideally we provide additional context here somehow...
    throw std::runtime_error("Cannot instantiate TableReader");
  }

  std::shared_ptr<arrow::Table> table;
  // Read table from JSON file
  status = reader->Read(&table);
  if (!status.ok()) {
    // Ideally we provide additional context here somehow...
    throw std::runtime_error("An error has occured while parsing JSON data");
  }

  return table;
}
