//
// Created by MichaelBrunner on 15/07/2024.
//

#include "GtfsCalendarReaderCsv.h"

#include "GtfsCsvHelpers.h"
#include "src/utils/utils.h"

#include <array>
#include <csv2/reader.hpp>


namespace schedule::gtfs {

  struct TempCalendar
  {
    std::string serviceId;
    std::string monday;
    std::string tuesday;
    std::string wednesday;
    std::string thursday;
    std::string friday;
    std::string saturday;
    std::string sunday;
    std::string start_date;
    std::string end_date;
  };

  GtfsCalendarReaderCsv::GtfsCalendarReaderCsv(std::string&& filename)
    : filename(std::move(filename)) {
    if (this->filename.empty())
    {
      throw std::invalid_argument("Filename is empty");
    }
  }
  GtfsCalendarReaderCsv::GtfsCalendarReaderCsv(std::string const& filename)
  : filename(filename) {
    if (this->filename.empty())
    {
      throw std::invalid_argument("Filename is empty");
    }
  }

  void GtfsCalendarReaderCsv::operator()(GtfsReader& aReader) const {
    auto reader = csv2::Reader();
    if (!reader.mmap(filename))
    {
      throw std::runtime_error("Cannot read file");
    }
    const auto header = reader.header();
    const std::vector<std::string> headerItems = utils::mapHeaderItemsToVector(header);

    std::map<size_t, std::string> headerMap = utils::createHeaderMap(headerItems);

    int index = 0;
    for (const auto& row : reader)
    {
      TempCalendar tempCalendarDate;
      index = 0;
      for (const auto& cell : row)
      {
        auto columnName = headerMap[index];

        std::string value;
        cell.read_value(value);
        value.erase(std::ranges::remove(value, '\r').begin(), value.end());
        value = utils::removeUtf8Bom(value);
        value = utils::removeQuotesFromStringView(value);

        static const std::map<std::string, std::function<void(TempCalendar&, const std::string&)>> columnActions = {
          {"service_id", [](TempCalendar& calendar, const std::string& val) { calendar.serviceId = val; }},
          {"monday", [](TempCalendar& calendar, const std::string& val) { calendar.monday = val; }},
          {"tuesday", [](TempCalendar& calendar, const std::string& val) { calendar.tuesday = val; }},
          {"wednesday", [](TempCalendar& calendar, const std::string& val) { calendar.wednesday = val; }},
          {"thursday", [](TempCalendar& calendar, const std::string& val) { calendar.thursday = val; }},
          {"friday", [](TempCalendar& calendar, const std::string& val) { calendar.friday = val; }},
          {"saturday", [](TempCalendar& calendar, const std::string& val) { calendar.saturday = val; }},
          {"sunday", [](TempCalendar& calendar, const std::string& val) { calendar.sunday = val; }},
          {"start_date", [](TempCalendar& calendar, const std::string& val) { calendar.start_date = val; }},
          {"end_date", [](TempCalendar& calendar, const std::string& val) { calendar.end_date = val; }},
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

        Calendar::WeekdayServiceHashMap weekdayService = {
          {std::chrono::Monday, std::stoi(tempCalendarDate.monday)},
          {std::chrono::Tuesday, std::stoi(tempCalendarDate.tuesday)},
          {std::chrono::Wednesday, std::stoi(tempCalendarDate.wednesday)},
          {std::chrono::Thursday, std::stoi(tempCalendarDate.thursday)},
          {std::chrono::Friday, std::stoi(tempCalendarDate.friday)},
          {std::chrono::Saturday, std::stoi(tempCalendarDate.saturday)},
          {std::chrono::Sunday, std::stoi(tempCalendarDate.sunday)}};

        aReader.getData().get().calendars.emplace(serviceId,
                                                      Calendar{std::move(tempCalendarDate.serviceId),
                                                               std::move(weekdayService),
                                                               std::move(tempCalendarDate.start_date),
                                                               std::move(tempCalendarDate.end_date)});
      }
    }
  }
  // gtfs
} // schedule