//
// Created by MichaelBrunner on 02/06/2024.
//

#ifndef CALENDAR_DATE_H
#define CALENDAR_DATE_H



#include <string>
#include <stdexcept>
#include <chrono>
#include <cstdint>
#include <model/helperFunctions.h>
#include <schedule_export.h>

// https://gtfs.org/schedule/reference/#calendar_datestxt

namespace schedule::gtfs {
  struct SCHEDULE_API CalendarDate
  {

    enum ExceptionType : uint8_t
    {
      SERVICE_ADDED = 1,
      SERVICE_REMOVED = 2
    };

    CalendarDate(std::string aServiceId, std::string&& aDate, ExceptionType const aExceptionType)
      : serviceId(std::move(aServiceId))
      , date(schedule::utils::parseDate(aDate))
      , exceptionType(aExceptionType) {
      if (serviceId.empty()
          || aDate.empty())
      {
        throw std::invalid_argument("Mandatory calendar date fields must not be empty");
      }
    }
    std::string serviceId;
    std::chrono::year_month_day date;
    ExceptionType exceptionType;
  };

  inline auto calendarDateHash = [](const Calendar& calendarDate) {
    return std::hash<std::string>{}(calendarDate.serviceId);
  };

  inline auto calendarDateEqual = [](const Calendar& lhs, const Calendar& rhs) {
    return lhs.serviceId == rhs.serviceId;
  };

} // gtfs

#endif //CALENDAR_DATE_H
