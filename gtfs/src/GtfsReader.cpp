//
// Created by MichaelBrunner on 08/06/2024.
//

#include "GtfsReader.h"
#include <algorithm>
#include <utility>
#if __has_include(<execution> )
#include <execution> // Apple Clang does not support this header
#define HAS_EXECUTION
#endif

gtfs::GtfsReader::GtfsReader(std::vector<GtfsStrategy<GtfsReader>>&& strategies)
  : strategies(std::move(strategies)) {
  if (this->strategies.empty())
  {
    throw std::invalid_argument("No strategies provided");
  }
}

void gtfs::GtfsReader::readData() {
#ifdef NDEBUG
  std::for_each(std::execution::par, strategies.begin(), strategies.end(), [this](const auto& strategy) {
    strategy(*this);
  });
#else
  std::ranges::for_each(strategies, [this](const auto& strategy) {
    strategy(*this);
  });
#endif

  // execute registered strategies
  /*#if defined(HAS_EXECUTION) && !(defined(__clang__) && defined(__apple_build_version__))
      std::for_each(std::execution::par, strategies.begin(), strategies.end(), [this](const auto& strategy) {
        strategy(*this);
      });
  #else
      std::for_each(strategies.begin(), strategies.end(), [this](const auto& strategy) {
        strategy(*this);
      });
  #endif*/
}
