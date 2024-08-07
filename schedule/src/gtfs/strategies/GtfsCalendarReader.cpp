//
// Created by MichaelBrunner on 02/06/2024.
//

#include "GtfsCalendarReader.h"

#include "LoggingPool.h"
#include "gtfs/GtfsReader.h"
#include "src/utils/utils.h"
#include "utils/scopedTimer.h"

#include <fstream>
#include <source_location>

namespace gtfs {

  void GtfsCalendarReader::operator()(GtfsReader& aReader) const {
    MEASURE_FUNCTION(std::source_location().file_name());
    std::ifstream infile(filename);
    if (!infile.is_open())
    {
      // TODO log error
      throw std::runtime_error("Error opening file: " + std::string(filename));
    }
    LoggingPool::getInstance(Target::CONSOLE)->info(fmt::format("Reading file: {}", filename));
    std::string line;
    std::getline(infile, line); // Skip header line
    std::vector<std::string_view> fields;
    fields.reserve(10);
    while (std::getline(infile, line))
    {
      fields = schedule::gtfs::utils::splitLineAndRemoveQuotes(line);
      if (fields.size() < 10)
      {
        // TODO: Handle error
        LoggingPool::getInstance(Target::CONSOLE)->error(fmt::format("Invalid calendar filename: {} line: {} GTFS data {}", std::source_location::current().file_name(), std::source_location::current().line(), line));
        continue;
      }

      schedule::gtfs::Calendar::WeekdayServiceHashMap weekdayService
        = {{std::chrono::Monday, std::stoi(std::string(fields[1]))},
           {std::chrono::Tuesday, std::stoi(std::string(fields[2]))},
           {std::chrono::Wednesday, std::stoi(std::string(fields[3]))},
           {std::chrono::Thursday, std::stoi(std::string(fields[4]))},
           {std::chrono::Friday, std::stoi(std::string(fields[5]))},
           {std::chrono::Saturday, std::stoi(std::string(fields[6]))},
           {std::chrono::Sunday, std::stoi(std::string(fields[7]))}};

      aReader.getData().get().calendars.emplace_back(std::string(fields[0]), std::move(weekdayService),
                                                     std::string(fields[8]), std::string(fields[9]));
    }
  }

  GtfsCalendarReader::GtfsCalendarReader(std::string filename)
    : filename(std::move(filename)) {}

} // gtfs