//
// Created by MichaelBrunner on 07/06/2024.
//

#include "GtfsTripReader.h"

#include "src/utils/utils.h"
#include "src/GtfsReader.h"

#include <fstream>

namespace gtfs {

  GtfsTripReader::GtfsTripReader(std::string filename)
    : filename(std::move(filename)) {
  }

  void GtfsTripReader::operator()(GtfsReader& aReader) const {
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
        continue;
      }

      constexpr uint8_t quoteSize = 2; // this is needed to remove the quotes from the fields
      std::string routeId = fields[0].substr(1, fields[0].size() - quoteSize);
      std::string serviceId = fields[1].substr(1, fields[1].size() - quoteSize);
      std::string tripId = fields[2].substr(1, fields[2].size() - quoteSize);

      aReader.getData().get().trips.emplace_back(std::move(routeId), std::move(serviceId), std::move(tripId));
    }
  }
} // gtfs