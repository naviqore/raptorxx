//
// Created by MichaelBrunner on 28/05/2024.
//

#include "GtfsReader.h"

#include <algorithm>
#include <iostream>
#include <utility>

namespace gtfs {

  GtfsReader::GtfsReader(std::vector<std::function<void(GtfsReader&)>>&& strategies)
    : strategies(std::move(strategies)) {
    // if (auto keys = std::views::keys(strategies);
    //     std::ranges::any_of(keys, std::mem_fn(&std::string::empty)))
    // {
    //   throw std::invalid_argument("Filename cannot be empty");
    // }
  }

  void GtfsReader::readData() {
    std::ranges::for_each(strategies, [this](const auto& strategy) {
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