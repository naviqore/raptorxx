//
// Created by MichaelBrunner on 26/05/2024.
//


// provides main(); this line is required in only one .cpp file
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

//function to be tested
int fact(int n) {
  return n <= 1 ? n : fact(n - 1) * n;
}

TEST_CASE("testing the factorial function") {
  CHECK(fact(1) == 1);
  CHECK(fact(2) == 2);
  CHECK(fact(3) == 6);
  CHECK(fact(10) == 3628800);
  // CHECK(fact(0) == 1); // should fail
}
