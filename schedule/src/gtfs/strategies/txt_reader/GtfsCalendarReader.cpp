//
// Created by MichaelBrunner on 02/06/2024.
//

#include "GtfsCalendarReader.h"


#include "LoggerFactory.h"
#include "GtfsReader.h"
#include "src/utils/utils.h"
#include "utils/scopedTimer.h"

#include <fstream>
#include <source_location>
#include <format>

namespace schedule::gtfs {

  void GtfsCalendarReader::operator()(GtfsReader& aReader) const {
    MEASURE_FUNCTION(std::source_location().file_name());
    std::ifstream infile(filename);
    if (!infile.is_open())
    {
      // TODO log error
      throw std::runtime_error("Error opening file: " + std::string(filename));
    }
    getLogger(Target::CONSOLE, LoggerName::GTFS)->info(std::format("Reading file: {}", filename));
    std::string line;
    std::getline(infile, line); // Skip header line
    std::map<std::string, size_t> headerMap = utils::getGtfsColumnIndices(line);
    std::vector<std::string_view> fields;
    fields.reserve(10);
    while (std::getline(infile, line))
    {
      fields = utils::splitLineAndRemoveQuotes(line);
      if (fields.size() < 10)
      {
        // TODO: Handle error
        getLogger(Target::CONSOLE, LoggerName::GTFS)->error(std::format("Invalid calendar filename: {} line: {} GTFS data {}", std::source_location::current().file_name(), std::source_location::current().line(), line));
        continue;
      }
      Calendar::WeekdayServiceHashMap weekdayService
        = {{std::chrono::Monday, std::stoi(std::string(fields[headerMap["monday"]]))},
           {std::chrono::Tuesday, std::stoi(std::string(fields[headerMap["tuesday"]]))},
           {std::chrono::Wednesday, std::stoi(std::string(fields[headerMap["wednesday"]]))},
           {std::chrono::Thursday, std::stoi(std::string(fields[headerMap["thursday"]]))},
           {std::chrono::Friday, std::stoi(std::string(fields[headerMap["friday"]]))},
           {std::chrono::Saturday, std::stoi(std::string(fields[headerMap["saturday"]]))},
           {std::chrono::Sunday, std::stoi(std::string(fields[headerMap["sunday"]]))}};

      aReader.getData().get().calendars.emplace(std::string(fields[headerMap["service_id"]]), Calendar(std::string(fields[headerMap["service_id"]]),
                                                                                                       std::move(weekdayService),
                                                                                                       std::string(fields[headerMap["start_date"]]),
                                                                                                       std::string(fields[headerMap["end_date"]])));
    }
  }

  GtfsCalendarReader::GtfsCalendarReader(std::string filename)
    : filename(std::move(filename)) {}

} // gtfs