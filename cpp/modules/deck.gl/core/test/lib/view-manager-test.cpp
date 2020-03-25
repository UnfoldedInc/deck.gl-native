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

#include "deck.gl/core/src/lib/view-manager.h"

#include <gtest/gtest.h>

#include <memory>

using namespace std;
using namespace deckgl;

namespace {

TEST(ViewManager, Construct) {
  auto viewManager = make_shared<ViewManager>();

  EXPECT_TRUE(viewManager != nullptr);
}

TEST(ViewManager, RedrawFlag) {
  auto viewManager = make_shared<ViewManager>();

  // Initialized needing redraw
  EXPECT_TRUE(viewManager->getNeedsRedraw());
  // Didn't clear redraw
  EXPECT_TRUE(viewManager->getNeedsRedraw(true));
  // Did clear redraw
  EXPECT_FALSE(viewManager->getNeedsRedraw());

  // Changing geometry causes redraw
  viewManager->setSize(10, 20);
  EXPECT_EQ(viewManager->width, 10);
  EXPECT_EQ(viewManager->height, 20);
  EXPECT_TRUE(viewManager->getNeedsRedraw(true));

  // No-op geometry change does not cause redraw
  viewManager->setSize(10, 20);
  EXPECT_FALSE(viewManager->getNeedsRedraw(true));

  // Manual redraw causes redraw
  viewManager->setNeedsUpdate("Test");
  EXPECT_TRUE(viewManager->getNeedsRedraw(true));
}

}  // namespace
