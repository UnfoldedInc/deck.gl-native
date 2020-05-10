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

#include "deck.gl/core/src/lib/earcut.hpp"

#include <gtest/gtest.h>

#include <array>
#include <memory>
#include <vector>

TEST(Earcut, IndicesLength) {
  using Coord = double;
  using N = uint16_t;

  using Point = std::array<Coord, 2>;
  std::vector<std::vector<Point>> polygon;
  std::vector<Point> mainPolygon = {{100, 0}, {100, 100}, {0, 100}, {0, 0}};
  std::vector<Point> holes = {{75, 25}, {75, 75}, {25, 75}, {25, 25}};

  polygon.push_back(mainPolygon);
  polygon.push_back(holes);

  std::vector<N> indices = mapbox::earcut<N>(polygon);

  // Length of indices list should be 3* that of the input polygon list
  EXPECT_EQ(indices.size() / 3, (mainPolygon.size() + holes.size()));
}

TEST(Earcut, IndicesLength3) {
  using Coord = double;
  using N = uint16_t;

  using Point = std::array<Coord, 3>;
  std::vector<std::vector<Point>> polygon;
  std::vector<Point> mainPolygon = {{100, 0, 1}, {100, 100, 2}, {0, 100, 3}, {0, 0, 4}};
  std::vector<Point> holes = {{75, 25, 1}, {75, 75, 2}, {25, 75, 3}, {25, 25, 4}};

  polygon.push_back(mainPolygon);
  polygon.push_back(holes);

  std::vector<N> indices = mapbox::earcut<N>(polygon);

  // Length of indices list should be 3* that of the input polygon list
  EXPECT_EQ(indices.size() / 3, (mainPolygon.size() + holes.size()));
}
