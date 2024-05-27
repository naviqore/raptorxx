//
// Created by MichaelBrunner on 26/05/2024.
//


#include "gtest/gtest.h"
#include "TestObject.h"

auto add(const int a, const int b) {
  return a + b;
}


TEST(MathTest, Addition) {
  TestObject lTest = TestObject();
  lTest.testFunction();
  EXPECT_EQ(add(5, 8), 13);
  EXPECT_EQ(add(5, -8), -3);
}
