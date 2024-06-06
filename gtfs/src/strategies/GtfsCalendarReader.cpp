//
// Created by MichaelBrunner on 02/06/2024.
//

#include "GtfsCalendarReader.h"

#include "LoggingPool.h"
#include "src/GtfsReader.h"
#include "src/utils/utils.h"

#include <fstream>
#include <map>
#include <source_location>

namespace gtfs {

  void GtfsCalendarReader::operator()(GtfsReader& aReader) const {

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
      if (fields.size() != 10)
      {
        LoggingPool::getLogger()->error("Invalid calendar filename: {} line: {} GTFS data {}", std::source_location::current().file_name(), std::source_location::current().line(), line);
        continue;
      }
      constexpr uint8_t quoteSize = 2; // this is needed to remove the quotes from the fields
      std::string serviceId = fields[0].substr(1, fields[0].size() - quoteSize);
      Calendar::WeekdayServiceHashMap weekdayService = {
        {std::chrono::Monday, std::stoi(fields[1].substr(1, fields[1].size() - quoteSize))},
        {std::chrono::Tuesday, std::stoi(fields[2].substr(1, fields[2].size() - quoteSize))},
        {std::chrono::Wednesday, std::stoi(fields[3].substr(1, fields[3].size() - quoteSize))},
        {std::chrono::Thursday, std::stoi(fields[4].substr(1, fields[4].size() - quoteSize))},
        {std::chrono::Friday, std::stoi(fields[5].substr(1, fields[5].size() - quoteSize))},
        {std::chrono::Saturday, std::stoi(fields[6].substr(1, fields[6].size() - quoteSize))},
        {std::chrono::Sunday, std::stoi(fields[7].substr(1, fields[7].size() - quoteSize))}};
      std::string startDate = fields[8].substr(1, fields[8].size() - quoteSize);
      std::string endDate = fields[9].substr(1, fields[9].size() - quoteSize);
      aReader.getData().calendars.emplace_back(std::move(serviceId), std::move(weekdayService), std::move(startDate), std::move(endDate));
    }
  }

  GtfsCalendarReader::GtfsCalendarReader(std::string filename)
    : filename(std::move(filename)) {
  }

} // gtfs