//
// Created by MichaelBrunner on 14/07/2024.
//

#include "GtfsCalendarDateReaderCsvParser.h"

#include "csv_wrapper.h"

#include <utility>
#include <ranges>

namespace schedule::gtfs {
  GtfsCalendarDateReaderCsvParser::GtfsCalendarDateReaderCsvParser(std::string&& filename)
    : filename(std::move(filename)) {
    if (filename.empty())
    {
      throw std::invalid_argument("Filename is empty");
    }
  }
  GtfsCalendarDateReaderCsvParser::GtfsCalendarDateReaderCsvParser(std::string const& filename)
    : filename(filename) {
    if (this->filename.empty())
    {
      throw std::invalid_argument("Filename is empty");
    }
  }
  void GtfsCalendarDateReaderCsvParser::operator()(GtfsReader& aReader) const {
    auto reader = io::CSVReader<3, io::trim_chars<'"'>, io::double_quote_escape<',', '\"'>>(filename); // , io::trim_chars<'"'>
    reader.set_header("service_id", "date", "exception_type");
    std::string serviceId;
    std::string date;
    std::string exceptionType;
    reader.read_header(io::ignore_no_column, "service_id", "date", "exception_type"); //
    while (reader.read_row(serviceId, date, exceptionType))
    {

      auto temp = serviceId;
      aReader.getData().get().calendarDates[temp].emplace_back(
        std::move(serviceId),
        std::move(date),
        exceptionType == "1" ? CalendarDate::SERVICE_ADDED
                             : CalendarDate::SERVICE_REMOVED);
    }
  }
}