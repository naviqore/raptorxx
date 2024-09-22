//
// Created by MichaelBrunner on 06/06/2024.
//

#include "GtfsCalendarDateReader.h"

#include "LoggerFactory.h"
#include "GtfsReader.h"
#include "src/utils/scopedTimer.h"
#include "src/utils/utils.h"

#include <algorithm>
#include <fstream>
#include <source_location>


namespace schedule::gtfs {

  GtfsCalendarDateReader::GtfsCalendarDateReader(std::string filename)
    : filename(std::move(filename))
  {
  }

  void GtfsCalendarDateReader::operator()(GtfsReader& aReader) const
  {
    MEASURE_FUNCTION();
    std::ifstream infile(filename);
    if (!infile.is_open()) {
      throw std::runtime_error("Error opening file: " + std::string(filename));
    }
    getLogger(Target::CONSOLE, LoggerName::GTFS)->info(std::format("Reading file: {}", filename));
    constexpr size_t bufferSize = 1 << 20; // 1 MB buffer size
    std::vector<char> buffer(bufferSize);
    infile.rdbuf()->pubsetbuf(buffer.data(), bufferSize);

    std::string line;
    std::getline(infile, line); // Skip header line
    std::map<std::string, size_t> headerMap = utils::getGtfsColumnIndices(line);

    std::vector<std::string_view> fields;
    fields.reserve(3);
    while (std::getline(infile, line)) {
      fields = utils::splitLineAndRemoveQuotes(line);
      if (fields.size() < 3) {
        continue;
      }

      CalendarDate::ExceptionType exceptionType;
      switch (fields[headerMap["exception_type"]][0]) {
        case '1':
          exceptionType = CalendarDate::ExceptionType::SERVICE_ADDED;
          break;
        case '2':
          exceptionType = CalendarDate::ExceptionType::SERVICE_REMOVED;
          break;
        default:
          exceptionType = CalendarDate::ExceptionType::SERVICE_REMOVED;
      }

      aReader.getData().get().calendarDates[std::string(fields[headerMap["service_id"]])].emplace_back(
        std::make_shared<CalendarDate>(std::string(fields[headerMap["service_id"]]),
                                       std::string(fields[headerMap["date"]]),
                                       exceptionType));
    }
  }
} // gtfs
