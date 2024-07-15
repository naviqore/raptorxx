//
// Created by MichaelBrunner on 14/07/2024.
//

#include "GtfsCalendarDateReaderCsv.h"

#include "GtfsCsvHelpers.h"

#include <utility>
#include <csv2/reader.hpp>
#include <ranges>

namespace schedule::gtfs {
  struct TempCalendarDate
  {
    std::string serviceId;
    std::string date;
    CalendarDate::ExceptionType exceptionType = CalendarDate::SERVICE_REMOVED;
  };


  GtfsCalendarDateReaderCsv::GtfsCalendarDateReaderCsv(std::string&& filename)
    : filename(std::move(filename)) {
    if (filename.empty())
    {
      throw std::invalid_argument("Filename is empty");
    }
  }
  void GtfsCalendarDateReaderCsv::operator()(GtfsReader& aReader) const {
    auto reader = csv2::Reader();

    if (false == reader.mmap(filename))
    {
      throw std::runtime_error("can not read file");
    }
    const auto header = reader.header();
    const std::vector<std::string> headerItems = utils::mapHeaderItemsToVector(header);

    std::map<size_t, std::string> headerMap = utils::createHeaderMap(headerItems);

    int index = 0;
    for (const auto& row : reader)
    {
      TempCalendarDate tempCalendarDate;
      index = 0;
      for (const auto& cell : row)
      {
        auto columnName = headerMap[index];

        std::string value;
        cell.read_value(value);
        value.erase(std::ranges::remove(value, '\r').begin(), value.end());

        static const std::map<std::string, std::function<void(TempCalendarDate&, const std::string&)>> columnActions = {
          {"service_id", [](TempCalendarDate& calendarDate, const std::string& val) { calendarDate.serviceId = val; }},
          {"date", [](TempCalendarDate& calendarDate, const std::string& val) { calendarDate.date = val; }},
          {"exception_type", [](TempCalendarDate& calendarDate, const std::string& val) { calendarDate.exceptionType = val == "1" ? CalendarDate::SERVICE_ADDED
                                                                                                                                  : CalendarDate::SERVICE_REMOVED; }},
        };

        if (columnActions.contains(columnName))
        {
          columnActions.at(columnName)(tempCalendarDate, value);
        }
        ++index;
      }
      if (!tempCalendarDate.serviceId.empty())
      {
        auto serviceId = tempCalendarDate.serviceId;
        aReader.getData().get().calendarDates[serviceId].emplace_back(
          std::move(tempCalendarDate.serviceId),
          std::move(tempCalendarDate.date),
          tempCalendarDate.exceptionType);
      }
    }
  }
}