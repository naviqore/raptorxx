//
// Created by MichaelBrunner on 28/05/2024.
//

#include "GtfsReader.h"

#include <utility>

namespace gtfs {

  GtfsReader::GtfsReader(const std::string_view filename, GtfsStrategy&& strategy)
    : filename(filename)
    , strategy(std::move(strategy)) {

    /* Checking that the given filename is valid and that
       the given 'std::function' instance is not empty */
  }


  void GtfsReader::readData() {
    strategy(*this);
  }
} // gtfs