//
// Created by MichaelBrunner on 02/06/2024.
//

#ifndef CALENDAR_DATE_H
#define CALENDAR_DATE_H



#include <string>
#include <stdexcept>
#include <chrono>

// https://gtfs.org/schedule/reference/#calendar_datestxt

namespace gtfs {
  struct CalendarDate
  {
    CalendarDate(std::string&& aServiceId, std::string&& aDate, int aExceptionType)
      : serviceId(std::move(aServiceId))
      , date(utils::parseDate(aDate))
      , exceptionType(aExceptionType) {
      if (serviceId.empty()
          || aDate.empty())
      {
        throw std::invalid_argument("Mandatory calendar date fields must not be empty");
      }
    }
    std::string serviceId;
    std::chrono::year_month_day date;
    int exceptionType;
  };

} // gtfs

#endif //CALENDAR_DATE_H
