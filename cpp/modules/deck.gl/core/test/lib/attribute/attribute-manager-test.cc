#include "deck.gl/core/src/lib/attribute/attribute-manager.h"

#include <gtest/gtest.h>

namespace {

/**
 * The fixture for testing class AttributeManager.
 */
class AttributeManagerTest : public ::testing::Test {
 protected:
  AttributeManagerTest() : manager{0, "identifier"} {
    // You can do set-up work for each test here.
  }

  deckgl::AttributeManager manager;
};

// Tests that the AttributeManager initializes properly.
TEST_F(AttributeManagerTest, Initialization) { EXPECT_EQ(manager.identifier, "identifier"); }

// Tests that AttributeManager sets redraw flag properly.
TEST_F(AttributeManagerTest, Redraw) {
  EXPECT_FALSE(manager.getNeedsRedraw());
  manager.setNeedsRedraw();
  EXPECT_TRUE(manager.getNeedsRedraw());
}

}  // namespace
