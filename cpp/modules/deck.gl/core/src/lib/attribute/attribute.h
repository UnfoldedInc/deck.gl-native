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

#ifndef DECKGL_CORE_LIB_ATTRIBUTE_ATTRIBUTE_H
#define DECKGL_CORE_LIB_ATTRIBUTE_ATTRIBUTE_H

#include <arrow/array.h>

#include <memory>
#include <string>
#include <utility>

#include "../../arrow/row.h"

namespace deckgl {

using namespace deckgl;

struct AttributeDescriptor {
 public:
  AttributeDescriptor(const std::string& name, const std::shared_ptr<arrow::DataType>& type)
      : name{std::move(name)}, type{type} {}

  std::string name;
  std::shared_ptr<arrow::DataType> type;

  // TODO(ilija): This doesn't work, and we can't really create a class template as we wouldn't be able to make use of
  // it in attribute manager
  //  template<typename ReturnValue>
  //  std::function<ReturnValue(TableRow)> accessor;
};

}  // namespace deckgl

#endif  // DECKGL_CORE_LIB_ATTRIBUTE_ATTRIBUTE_H
