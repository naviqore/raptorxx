//
// Created by MichaelBrunner on 07/06/2024.
//

#include "GtfsRouteReader.h"

#include "LoggingPool.h"
#include "src/GtfsReader.h"
#include "src/utils/utils.h"
#include <fstream>
#include <map>
#include <source_location>
#include <Coordinate.h>

namespace gtfs {
  void GtfsRouteReader::operator()(GtfsReader& aReader) const {

    std::ifstream infile(filename);
    if (!infile.is_open())
    {
      // TODO log error
      throw std::runtime_error("Error opening file: " + std::string(filename));
    }
    LoggingPool::getLogger()->info("Reading file: {}", filename);
    std::string line;
    std::getline(infile, line); // Skip header line

    while (std::getline(infile, line))
    {
      auto fields = utils::splitLine(line);
      if (constexpr uint8_t expectedNumberOfFields = 6; fields.size() != expectedNumberOfFields)
      {
        LoggingPool::getLogger()->error("Invalid route filename: {} line: {} GTFS data {}", std::source_location::current().file_name(), std::source_location::current().line(), line);
        continue;
      }
      constexpr uint8_t quoteSize = 2; // this is needed to remove the quotes from the fields
      std::string stopId = fields[0].substr(1, fields[0].size() - quoteSize);
      std::string stopName = fields[1].substr(1, fields[1].size() - quoteSize);
      const double stopLat = std::stod(fields[2].substr(1, fields[2].size() - quoteSize));
      const double stopLon = std::stod(fields[3].substr(1, fields[3].size() - quoteSize));
      // TODO set fields if needed
      std::string locationType = fields[4].substr(1, fields[4].size() - quoteSize);
      std::string parentStation = fields[5].substr(1, fields[5].size() - quoteSize);
      aReader.getData().stops.emplace_back(std::move(stopId), std::move(stopName), geometry::Coordinate(stopLat), geometry::Coordinate(stopLon));
    }
  }

  GtfsRouteReader::GtfsRouteReader(std::string filename)
    : filename(std::move(filename)) {
  }
} // gtfs


