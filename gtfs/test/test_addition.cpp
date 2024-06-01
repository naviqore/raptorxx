//
// Created by MichaelBrunner on 26/05/2024.
//


#include <gtest/gtest.h>
#include <TestObject.h>
#include <ProjectConfig.h>


#include <cstdint>

uint64_t fibonacci(uint64_t number) {
  return number < 2 ? number : fibonacci(number - 1) + fibonacci(number - 2);
}


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