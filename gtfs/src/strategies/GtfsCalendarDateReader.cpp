//
// Created by MichaelBrunner on 06/06/2024.
//

#include "GtfsCalendarDateReader.h"

#include "src/GtfsReader.h"
#include "src/utils/utils.h"

#include <fstream>
#include <stdexcept>
#include <exception>
#include <expected>

namespace gtfs {

  GtfsCalendarDateReader::GtfsCalendarDateReader(std::string filename)
    : filename(std::move(filename)) {
  }

  void GtfsCalendarDateReader::operator()(GtfsReader& aReader) const {
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
      if (fields.size() < 3)
      {
        // auto result = std::unexpected("Error: insufficient number of fields.");
        // TODO: Handle error
        continue;
      }
      constexpr uint8_t quoteSize = 2; // this is needed to remove the quotes from the fields
      std::string serviceId = fields[0].substr(1, fields[0].size() - quoteSize);
      std::string date = fields[1].substr(1, fields[1].size() - quoteSize);
      int exceptionType = std::stoi(fields[2].substr(1, fields[2].size() - quoteSize));

      aReader.getData().calendarDates.emplace_back(std::move(serviceId), std::move(date), exceptionType);
    }
  }

} // gtfs