//
// Created by MichaelBrunner on 06/06/2024.
//

#include <gtest/gtest.h>

int main(int argc, char** argv)
{
  // testing::GTEST_FLAG(filter) = "GtfsTripReaderTest.*";
  // testing::GTEST_FLAG(break_on_failure) = true;
  // testing::GTEST_FLAG(color) = "yes";
  // testing::GTEST_FLAG(throw_on_failure) = true;
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}