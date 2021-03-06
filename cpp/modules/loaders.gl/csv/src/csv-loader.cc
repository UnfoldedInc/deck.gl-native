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

#include "./csv-loader.h"  // NOLINT(build/include)

#include <arrow/api.h>
#include <arrow/csv/api.h>

using namespace loadersgl;

auto CSVLoader::loadTable(const std::shared_ptr<arrow::io::InputStream> input, probegl::Error& error) noexcept
    -> std::shared_ptr<arrow::Table> {
  return probegl::catchError<arrow::Table>([&]() { return this->loadTable(input); }, error);
}

auto CSVLoader::loadTable(const std::shared_ptr<arrow::io::InputStream> input) -> std::shared_ptr<arrow::Table> {
  arrow::MemoryPool* pool = arrow::default_memory_pool();

  auto readOptions = arrow::csv::ReadOptions::Defaults();
  auto parseOptions = arrow::csv::ParseOptions::Defaults();
  auto convertOptions = arrow::csv::ConvertOptions::Defaults();

  // Instantiate TableReader from input stream and options
  auto makeResult = arrow::csv::TableReader::Make(pool, input, readOptions, parseOptions, convertOptions);
  if (!makeResult.ok()) {
    // TODO(ilija@unfolded.ai): Ideally we provide additional context here somehow...
    throw std::runtime_error("Cannot instantiate TableReader");
  }

  std::shared_ptr<arrow::csv::TableReader> reader = makeResult.ValueOrDie();

  // Read table from CSV file
  auto readResult = reader->Read();
  if (!readResult.ok()) {
    // TODO(ilija@unfolded.ai): Ideally we provide additional context here somehow...
    throw std::runtime_error("An error has occured while parsing CSV data");
  }
  std::shared_ptr<arrow::Table> table = readResult.ValueOrDie();

  return table;
}
