//
// Created by MichaelBrunner on 07/06/2024.
//

#include "GtfsStopTimeReader.h"

#include "src/GtfsReader.h"
#include "src/utils/utils.h"

#include <fstream>
#include <stdexcept>

namespace gtfs {

  void GtfsStopTimeReader::operator()(GtfsReader& aReader) const {
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
      std::string tripId = fields[0].substr(1, fields[0].size() - quoteSize);
      std::string arrivalTime = fields[1].substr(1, fields[1].size() - quoteSize);
      std::string departureTime = fields[2].substr(1, fields[2].size() - quoteSize);
      std::string stopId = fields[3].substr(1, fields[3].size() - quoteSize);
      const int stopSequence = std::stoi(fields[4].substr(1, fields[4].size() - quoteSize));
      //int pickupType = std::stoi(fields[5].substr(1, fields[5].size() - quoteSize));
      //int dropOffType = std::stoi(fields[6].substr(1, fields[6].size() - quoteSize));

      aReader.getData().get().stopTimes.emplace_back(std::move(tripId), std::move(arrivalTime), std::move(departureTime),
                                               std::move(stopId), stopSequence);
    }
  }

  GtfsStopTimeReader::GtfsStopTimeReader(std::string filename)
    : filename(std::move(filename)) {
  }
} // gtfs