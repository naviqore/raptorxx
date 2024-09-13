//
// Created by MichaelBrunner on 02/06/2024.
//

#ifndef CALENDAR_H
#define CALENDAR_H


#include <string>
#include <stdexcept>
#include <chrono>
#include <unordered_map>
#include <model/helperFunctions.h>
#include <schedule_export.h>

inline auto weekdayHash = [](const std::chrono::weekday& wd) {
  return static_cast<size_t>(wd.c_encoding());
};

namespace schedule::gtfs {
  struct SCHEDULE_API Calendar
  {
    using WeekdayServiceHashMap = std::unordered_map<std::chrono::weekday, int, decltype(weekdayHash), std::equal_to<>>;

    Calendar(std::string aServiceId, WeekdayServiceHashMap&& aWeekdayService, std::string&& aStartDate, std::string&& aEndDate)
      : serviceId(std::move(aServiceId))
      , weekdayService(std::move(aWeekdayService))
      , startDate(schedule::utils::parseDate(aStartDate))
      , endDate(schedule::utils::parseDate(aEndDate)) {
      if (serviceId.empty()
          || aStartDate.empty()
          || aEndDate.empty())
      {
        throw std::invalid_argument("Mandatory calendar fields must not be empty");
      }
    }
    std::string serviceId;
    WeekdayServiceHashMap weekdayService;
    std::chrono::year_month_day startDate;
    std::chrono::year_month_day endDate;
  };

  inline auto calendarHash = [](const Calendar& calendar) {
    return std::hash<std::string>{}(calendar.serviceId);
  };

  inline auto calendarEqual = [](const Calendar& lhs, const Calendar& rhs) {
    return lhs.serviceId == rhs.serviceId;
  };

}

#endif //CALENDAR_H
