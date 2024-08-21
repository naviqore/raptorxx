//
// Created by MichaelBrunner on 26/07/2024.
//

#pragma once
#include <limits>

namespace raptor::types {
  static constexpr auto INFINITY_VALUE_MAX = std::numeric_limits<unsigned int>::max();
  static constexpr auto INFINITY_VALUE_MIN = std::numeric_limits<unsigned int>::min();

  static constexpr int NO_INDEX = -1;

  using raptorInt = unsigned int;
  using raptorIdx = unsigned int;
}