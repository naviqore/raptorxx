//
// Created by MichaelBrunner on 28/05/2024.
//

#include "GtfsReader.h"

#include <algorithm>
#include <utility>

#if __has_include(<execution> )
#include <execution> // Apple Clang does not support this header
#define HAS_EXECUTION
#endif

namespace gtfs {

  GtfsReader::GtfsReader(std::vector<GtfsStrategy<GtfsReader>>&& strategies)
    : strategies(std::move(strategies)) {
    if (this->strategies.empty())
    {
      throw std::invalid_argument("No strategies provided");
    }
  }

  void GtfsReader::readData() {
#if defined(HAS_EXECUTION) && !(defined(__clang__) && defined(__apple_build_version__))
    std::for_each(std::execution::par, strategies.begin(), strategies.end(), [this](const auto& strategy) {
      strategy(*this);
    });
#else
    std::for_each(strategies.begin(), strategies.end(), [this](const auto& strategy) {
      strategy(*this);
    });
#endif
  }

  const GtfsData& GtfsReader::getData() const {
    return data;
  }
  GtfsData& GtfsReader::getData() {
    return data;
  }
} // gtfs