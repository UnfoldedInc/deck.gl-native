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

TEST(ViewManager, SetViews) {
  auto viewManager = make_shared<ViewManager>();
  // Clear redraw flag
  viewManager->getNeedsRedraw(true);

  auto view1 = make_shared<View>();
  view1->setProperty("width", 100);
  string view1Id = "view1";
  view1->setProperty("id", view1Id);
  auto view2 = make_shared<View>();
  view2->setProperty("height", 200);
  string view2Id = "view2";
  view2->setProperty("id", view2Id);

  // Setting view causes redraw
  viewManager->setViews({view1});
  EXPECT_TRUE(viewManager->getNeedsRedraw(true));
  EXPECT_EQ(size_t{1}, viewManager->getViews().size());
  // Same view does not cause redraw
  viewManager->setViews({view1});
  EXPECT_FALSE(viewManager->getNeedsRedraw());
  EXPECT_EQ(size_t{1}, viewManager->getViews().size());
  // Adding a view causes redraw
  viewManager->setViews({view1, view2});
  EXPECT_TRUE(viewManager->getNeedsRedraw(true));
  EXPECT_EQ(size_t{2}, viewManager->getViews().size());
  // Ordering matters
  viewManager->setViews({view2, view1});
  EXPECT_TRUE(viewManager->getNeedsRedraw(true));
  EXPECT_EQ(size_t{2}, viewManager->getViews().size());

  EXPECT_EQ(view2, *(viewManager->getViews().begin()));
  EXPECT_EQ(view1, *(++(viewManager->getViews().begin())));
  EXPECT_EQ(view2, viewManager->getView("view2"));
  EXPECT_EQ(view1, viewManager->getView("view1"));
  EXPECT_EQ(nullptr, viewManager->getView("view doesn't exist"));
}

}  // namespace
