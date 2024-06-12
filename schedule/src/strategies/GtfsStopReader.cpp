//
// Created by MichaelBrunner on 07/06/2024.
//

// GtfsStopReader.cpp
#include "GtfsStopReader.h"

#include "LoggingPool.h"
#include "src/GtfsReader.h"
#include "src/utils/utils.h"

#include <fstream>
#include <map>
#include <source_location>

namespace gtfs {

  void GtfsStopReader::operator()(GtfsReader& aReader) const {
    std::ifstream infile(filename);
    if (!infile.is_open())
    {
      throw std::runtime_error("Error opening file: " + std::string(filename));
    }

    std::string line;
    std::getline(infile, line); // Skip header line

    while (std::getline(infile, line))
    {
      auto fields = utils::splitLine(line);
      if (constexpr uint8_t expectedNumberOfFields = 7; fields.size() != expectedNumberOfFields)
      {
        // TODO: Handle error
        continue;
      }

      constexpr uint8_t quoteSize = 2; // this is needed to remove the quotes from the fields
      std::string stopId = fields[0].substr(1, fields[0].size() - quoteSize);
      std::string stopName = fields[1].substr(1, fields[1].size() - quoteSize);
      const double stopLat = std::stod(fields[2].substr(1, fields[2].size() - quoteSize));
      const double stopLon = std::stod(fields[3].substr(1, fields[3].size() - quoteSize));

      aReader.getData().get().stops.emplace_back(std::move(stopId), std::move(stopName), geometry::Coordinate<double>(stopLat), geometry::Coordinate<double>(stopLon));
    }
  }

  GtfsStopReader::GtfsStopReader(std::string filename)
    : filename(std::move(filename)) {
  }
} // gtfs
