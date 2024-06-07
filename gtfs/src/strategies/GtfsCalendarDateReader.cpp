//
// Created by MichaelBrunner on 06/06/2024.
//

#include "GtfsCalendarDateReader.h"

#include "src/GtfsReader.h"
#include "src/utils/utils.h"

#include <algorithm>
#include <fstream>
#include <stdexcept>
#include <exception>
#include <expected>


std::vector<std::string_view> splitLineAndRemoveQuotes(std::string_view line) {
  std::vector<std::string_view> result;
  size_t start = 0;
  size_t end = line.find(',');

  while (end != std::string_view::npos) {
    result.push_back(line.substr(start + 1, end - start - 2));
    start = end + 1;
    end = line.find(',', start);
  }

  result.push_back(line.substr(start + 1, line.size() - start - 2));
  return result;
}


namespace gtfs {

  GtfsCalendarDateReader::GtfsCalendarDateReader(std::string filename)
    : filename(std::move(filename)) {
  }

  void GtfsCalendarDateReader::operator()(GtfsReader& aReader) const {
    std::ifstream infile(filename);
    if (!infile.is_open()) {
      throw std::runtime_error("Error opening file: " + std::string(filename));
    }

    constexpr size_t bufferSize = 1 << 20; // 1 MB buffer size - this is used to speed up reading the file
    std::vector<char> buffer(bufferSize);
    infile.rdbuf()->pubsetbuf(buffer.data(), bufferSize);

    std::string line;
    std::getline(infile, line); // Skip header line

    // Reserve memory for vector
    constexpr size_t expectedSizec = 5'600'000;
    aReader.getData().calendarDates.reserve(expectedSizec);

    while (std::getline(infile, line)) {
      auto fields = splitLineAndRemoveQuotes(line);
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
          throw std::runtime_error("Error: invalid exception type.");
      }

      aReader.getData().calendarDates.emplace_back(
          std::string(fields[0]), std::string(fields[1]), exceptionType);
    }
  }

  // void GtfsCalendarDateReader::operator()(GtfsReader& aReader) const {
  //   std::ifstream infile(filename);
  //   if (!infile.is_open())
  //   {
  //     throw std::runtime_error("Error opening file: " + std::string(filename));
  //   }
  //
  //   std::string line;
  //   std::getline(infile, line); // Skip header line
  //
  //   // Reserve memory for vector
  //   constexpr size_t expectedSizec = 5'600'000; // Adjust as needed
  //   aReader.getData().calendarDates.reserve(expectedSizec);
  //
  //   // Constants moved outside the loop
  //   constexpr uint8_t quoteSize = 2;
  //   constexpr uint8_t expectedNumberOfFields = 3;
  //
  //   while (std::getline(infile, line))
  //   {
  //     auto fields = utils::splitLine(line);
  //     if (fields.size() < expectedNumberOfFields)
  //     {
  //       // TODO: Handle error
  //       continue;
  //     }
  //
  //     std::ranges::transform(fields, fields.begin(), [](const std::string& field) {
  //       return field.substr(1, field.size() - quoteSize);
  //     });
  //
  //     CalendarDate::ExceptionType exceptionType;
  //     switch (fields[2][0])
  //     {
  //       case '1':
  //         exceptionType = CalendarDate::ExceptionType::SERVICE_ADDED;
  //         break;
  //       case '2':
  //         exceptionType = CalendarDate::ExceptionType::SERVICE_REMOVED;
  //         break;
  //       default:
  //         throw std::runtime_error("Error: invalid exception type.");
  //     }
  //
  //     aReader.getData().calendarDates.emplace_back(
  //       std::move(fields[0]), std::move(fields[1]), exceptionType);
  //   }
  // }


  /*void GtfsCalendarDateReader::operator()(GtfsReader& aReader) const {
    std::ifstream infile(filename);
    if (!infile.is_open())
    {
      throw std::runtime_error("Error opening file: " + std::string(filename));
    }

    std::string line;
    std::getline(infile, line); // Skip header line

    // Reserve memory for vector
    constexpr size_t expectedSizec = 5'600'000;
    aReader.getData().calendarDates.reserve(expectedSizec);

    while (std::getline(infile, line))
    {
      auto fields = utils::splitLine(line);
      if (constexpr uint8_t expectedNumberOfFields = 3; fields.size() < expectedNumberOfFields)
      {
        // TODO: Handle error
        continue;
      }
      constexpr uint8_t quoteSize = 2; // this is needed to remove the quotes from the fields

      std::string serviceId = fields[0].substr(1, fields[0].size() - quoteSize);
      std::string date = fields[1].substr(1, fields[1].size() - quoteSize);
      std::string exceptionTypeStr = fields[2].substr(1, fields[2].size() - quoteSize);

      CalendarDate::ExceptionType exceptionType = exceptionTypeStr == "1"
                                                    ? CalendarDate::ExceptionType::SERVICE_ADDED
                                                  : exceptionTypeStr == "2"
                                                    ? CalendarDate::ExceptionType::SERVICE_REMOVED
                                                    : throw std::runtime_error("Error: invalid exception type.");


      aReader.getData().calendarDates.emplace_back(std::move(serviceId), std::move(date), exceptionType);
    }
  }*/

} // gtfs