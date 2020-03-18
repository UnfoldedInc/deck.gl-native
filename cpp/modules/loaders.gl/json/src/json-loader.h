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

#ifndef LOADERSGL_JSON_JSON_CONVERTER_H
#define LOADERSGL_JSON_JSON_CONVERTER_H

#include <arrow/io/interfaces.h>
#include <arrow/table.h>

#include <functional>  // {std::function}
#include <map>
#include <memory>  // {std::shared_ptr}

#include "deck.gl/core.h"  // {Component} // TODO - this is a "circular" dependency
#include "json/json.h"     // {Json::Value} (https://github.com/open-source-parsers/jsoncpp)

namespace loadersgl {

class JSONLoader {
 public:
  // methods
  JSONLoader() {}

  auto loadTable(const std::shared_ptr<arrow::io::InputStream> input) -> std::shared_ptr<arrow::Table>;
};

}  // namespace loadersgl

#endif  // LOADERSGL_JSON_JSON_CONVERTER_H
