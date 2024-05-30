//
// Created by MichaelBrunner on 26/05/2024.
//


#include <gtest/gtest.h>
#include <TestObject.h>
#include <ProjectConfig.h>

auto add(const int a, const int b) {
  return a + b;
}


TEST(MathTest, Addition) {
  TestObject lTest = TestObject();
  lTest.testFunction();
  std::cout << "v" << PROJECT_VERSION_MAJOR << "." << PROJECT_VERSION_MINOR << "." << PROJECT_VERSION_PATCH << std::endl;
  EXPECT_EQ(add(5, 8), 13);
  EXPECT_EQ(add(5, -8), -3);
}

TEST(GTFS, TestFunction) {
  testFunction();
}