//
// Created by MichaelBrunner on 28/05/2024.
//

#include "GtfsReader.h"

#include <algorithm>
#include <execution>
#include <utility>

namespace gtfs {

  GtfsReader::GtfsReader(std::vector<GtfsStrategy<GtfsReader>>&& strategies)
    : strategies(std::move(strategies)) {
    if (this->strategies.empty())
    {
      throw std::invalid_argument("No strategies provided");
    }
  }

  // void GtfsReader::readData() {
  //   std::ranges::for_each(strategies, [this](const auto& strategy) {
  //     strategy(*this);
  //   });

  void GtfsReader::readData() {
    std::for_each(std::execution::par, strategies.begin(), strategies.end(), [this](const auto& strategy) {
      strategy(*this);
    });
  }

  const GtfsData& GtfsReader::getData() const {
    return data;
  }
  GtfsData& GtfsReader::getData() {
    return data;
  }
} // gtfs