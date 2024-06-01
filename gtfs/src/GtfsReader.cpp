//
// Created by MichaelBrunner on 28/05/2024.
//

#include "GtfsReader.h"

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <utility>
#include <ranges>

namespace gtfs {

  /*GtfsReader::GtfsReader(const std::string_view filename, GtfsStrategy&& strategy)
    : filename(filename)
    , strategy(std::move(strategy)) {

    /* Checking that the given filename is valid and that
       the given 'std::function' instance is not empty #1#
    if (filename.empty())
    {
      throw std::invalid_argument("Filename cannot be empty");
    }
  }*/

  GtfsReader::GtfsReader(std::vector<std::function<void(GtfsReader&)>>&& strategies)
    : strategies(std::move(strategies)) {
    // if (auto keys = std::views::keys(strategies);
    //     std::ranges::any_of(keys, std::mem_fn(&std::string::empty)))
    // {
    //   throw std::invalid_argument("Filename cannot be empty");
    // }
  }

  void GtfsReader::readData() {
    // strategy(*this);
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