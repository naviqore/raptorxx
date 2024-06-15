//
// Created by MichaelBrunner on 06/06/2024.
//

#include "GtfsCalendarDateReader.h"

#include "gtfs/GtfsReader.h"
#include "src/utils/scopedTimer.h"
#include "src/utils/utils.h"

#include <algorithm>
#include <fstream>
#include <stdexcept>
#include <exception>
#include <expected>
#include <source_location>


namespace gtfs {

  GtfsCalendarDateReader::GtfsCalendarDateReader(std::string filename)
    : filename(std::move(filename)) {
  }

  void GtfsCalendarDateReader::operator()(GtfsReader& aReader) const {
    MEASURE_FUNCTION();
    std::ifstream infile(filename);
    if (!infile.is_open()) {
      // throw std::runtime_error("Error opening file: " + std::string(filename));
      return;
    }

    constexpr size_t bufferSize = 1 << 20; // 1 MB buffer size - this is used to speed up reading the file
    std::vector<char> buffer(bufferSize);
    infile.rdbuf()->pubsetbuf(buffer.data(), bufferSize);

    std::string line;
    std::getline(infile, line); // Skip header line

    // Reserve memory for vector
    constexpr size_t expectedSizec = 5'600'000;
    aReader.getData().get().calendarDates.reserve(expectedSizec);
    std::vector<std::string_view> fields;
    fields.reserve(3);
    while (std::getline(infile, line)) {
      fields = utils::splitLineAndRemoveQuotes(line);
      if (fields.size() < 3) {
        // TODO: Handle error
        continue;
      }

      CalendarDate::ExceptionType exceptionType;
      switch (fields[2][0]) {
        case '1':
          exceptionType = CalendarDate::ExceptionType::SERVICE_ADDED;
        break;
        case '2':
          exceptionType = CalendarDate::ExceptionType::SERVICE_REMOVED;
        break;
        default:
          break;
          //throw std::runtime_error("Error: invalid exception type.");
      }

      aReader.getData().get().calendarDates.emplace_back(
          std::string(fields[0]), std::string(fields[1]), exceptionType);
    }
  }
} // gtfs