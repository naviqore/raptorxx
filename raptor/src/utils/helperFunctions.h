//
// Created by MichaelBrunner on 04/08/2024.
//

#pragma once

#include <string>
#include <sstream>

namespace raptor::utils {
  template<typename Range>
  auto joinToString(const Range& range, const std::string& delimiter = " ") {
    std::ostringstream oss;
    for (const auto& elem : range)
    {
      if (oss.tellp() != 0)
      {
        oss << delimiter;
      }
      oss << elem;
    }
    return oss.str();
  }
} // raptor::utils