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

#ifndef DECKGL_CORE_LIB_ATTRIBUTE_DATA_COLUMN_H
#define DECKGL_CORE_LIB_ATTRIBUTE_DATA_COLUMN_H

#include <string>
#include <unordered_map>
#include <vector>

#include "luma.gl/core.h"
// TODO(ilija): Placeholder types
typedef void *UnknownType;
typedef void *BufferPlaceholder;
typedef void *ValuePlaceholder;
typedef void *AccessorPlaceholder;

namespace deckgl {

template <class DataType>
struct DataColumnOptions {
 public:
  DataColumnOptions(const std::string &identifier, int size, const DataType &defaultValue, bool isIndexed,
                    bool fp64 = true)
      : identifier(identifier), size(size), defaultValue(defaultValue), isIndexed(isIndexed), fp64(fp64) {}

  std::string identifier;
  int size;
  DataType defaultValue;
  bool isIndexed;
  bool fp64;

  // TODO(ilija): Not needed?
  //    void *logicalType;
  //    void *type;
};

/*
 * Represents a single column within a virtual data table.
 */
template <class DataType>
class DataColumn {
 public:
  DataColumn(wgpuDevice device, const DataColumnOptions<DataType> &options);

  bool setData(const UnknownType options);
  void updateSubBuffer(const UnknownType options);
  bool allocate(int numInstances, bool copy = false);

  BufferPlaceholder buffer();
  int byteOffset();
  void release();
  // TODO(ilija): Unfortunate naming as we already have these getter?
  BufferPlaceholder getBuffer();
  ValuePlaceholder getValue();
  AccessorPlaceholder getAccessor();

  wgpuDevice device;
  bool doublePrecision;
  DataType value;

  // TODO(ilija): Not needed?
  //    void *defaultType;
  //    void *settings;
  //    void *state;

 private:
  BufferPlaceholder *_buffer;

  void _checkExternalBuffer(UnknownType options);
  UnknownType _normalizeConstant(UnknownType value);
  UnknownType _normalizeValue(UnknownType value);
  template <class Value>
  bool _areValuesEqual(const Value &value1, const Value &value2);
};

}  // namespace deckgl

#endif  // DECKGL_CORE_LIB_ATTRIBUTE_DATA_COLUMN_H
