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

#include "luma.gl/garrow/src/util/arrow-utils.h"

#include <gtest/gtest.h>

using namespace lumagl::garrow;

namespace {

/// \brief LineLayer test suite.
class ArrowUtilsTestSuite : public ::testing::Test {
 protected:
  ArrowUtilsTestSuite() {}
};

TEST_F(ArrowUtilsTestSuite, ArrowTypeFromWebGPUFormat) {
  EXPECT_EQ(arrowTypeFromVertexFormat(wgpu::VertexFormat::Int), arrow::int32());
  EXPECT_EQ(arrowTypeFromVertexFormat(wgpu::VertexFormat::Float), arrow::float32());

  auto float4Format = arrowTypeFromVertexFormat(wgpu::VertexFormat::Float4);
  EXPECT_EQ(float4Format->id(), arrow::Type::type::FIXED_SIZE_LIST);
  EXPECT_EQ(float4Format->child(0)->type()->id(), arrow::Type::type::FLOAT);
}

TEST_F(ArrowUtilsTestSuite, VertexFormatFromArrowType) {
  EXPECT_EQ(vertexFormatFromArrowType(arrow::int32()).value(), wgpu::VertexFormat::Int);
  EXPECT_EQ(vertexFormatFromArrowType(arrow::float32()).value(), wgpu::VertexFormat::Float);
  EXPECT_EQ(vertexFormatFromArrowType(arrow::fixed_size_list(arrow::float32(), 4)).value(), wgpu::VertexFormat::Float4);
  EXPECT_EQ(vertexFormatFromArrowType(arrow::fixed_size_list(arrow::float32(), 5)), std::nullopt);
}

}  // anonymous namespace
