//
// Created by MichaelBrunner on 01/06/2024.
//

#include "GtfsAgencyReaderStrategy.h"

#include <stdexcept>

namespace gtfs {
  GtfsAgencyReaderStrategy::GtfsAgencyReaderStrategy(std::string_view filename)
    : filename(filename) {
    if (filename.empty())
    {
      throw std::invalid_argument("Filename cannot be empty");
    }
  }
} // gtfs