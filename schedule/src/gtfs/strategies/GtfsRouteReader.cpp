//
// Created by MichaelBrunner on 07/06/2024.
//

#include "GtfsRouteReader.h"

#include "LoggingPool.h"
#include "gtfs/GtfsReader.h"
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
      throw std::runtime_error("Error opening file: " + std::string(filename));
    }

    std::string line;
    std::getline(infile, line); // Skip header line

    while (std::getline(infile, line))
    {
      auto fields = utils::splitLine(line);
      if (constexpr uint8_t expectedNumberOfFields = 6; fields.size() != expectedNumberOfFields)
      {
        continue;
      }

      constexpr uint8_t quoteSize = 2; // this is needed to remove the quotes from the fields
      std::string routeId = fields[0].substr(1, fields[0].size() - quoteSize);
      // std::string agencyId = fields[1].substr(1, fields[1].size() - quoteSize);
      std::string routeShortName = fields[2].substr(1, fields[2].size() - quoteSize);
      std::string routeLongName = fields[3].substr(1, fields[3].size() - quoteSize);
      // std::string routeDesc = fields[4].substr(1, fields[4].size() - quoteSize);
      auto routeType = static_cast<Route::RouteType>(std::stoi(fields[5].substr(1, fields[5].size() - quoteSize)));

      aReader.getData().get().routes.emplace_back(std::move(routeId), std::move(routeShortName), std::move(routeLongName), routeType);
    }
  }

  GtfsRouteReader::GtfsRouteReader(std::string filename)
    : filename(std::move(filename)) {
  }
} // gtfs

