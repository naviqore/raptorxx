//
// Created by MichaelBrunner on 01/06/2024.
//

#ifndef GTFSDATA_H
#define GTFSDATA_H

#include "model/Agency.h"
#include "model/Calendar.h"
#include "model/CalendarDate.h"
#include "model/Route.h"
#include "model/Stop.h"
#include "model/Transfer.h"
#include "model/Trip.h"
#include "model/StopTime.h"


#include <vector>
#include <schedule_export.h>
#include <unordered_set>

namespace schedule::gtfs {

  struct SCHEDULE_API GtfsData
  {
    std::unordered_map<std::string, Agency /*, decltype(agencyHash), decltype(agencyEqual)*/> agencies = {};
    std::unordered_map<std::string, Calendar /*, decltype(calendarHash), decltype(calendarEqual)*/> calendars;
    std::unordered_map<std::string, std::vector<CalendarDate> /*, decltype(calendarDateHash), decltype(calendarDateEqual)*/> calendarDates;
    std::unordered_map<std::string, Route /*, decltype(routeHash), decltype(routeEqual)*/> routes;
    std::unordered_map<std::string, Stop /*, decltype(stopHash), decltype(stopEqual)*/> stops;
    std::unordered_map<std::string, StopTime /*, decltype(stopTimeHash), decltype(stopTimeEqual)*/> stopTimes;
    std::unordered_map<std::string, std::vector<Transfer> /*, decltype(transferHash), decltype(transferEqual)*/> transferFrom;
    std::unordered_map<std::string, std::vector<Transfer> /*, decltype(transferHash), decltype(transferEqual)*/> transferTo;
    std::unordered_map<std::string, std::vector<Trip> /*, decltype(tripHash), decltype(tripEqual)*/> trips;
  };

} // gtfs

#endif //GTFSDATA_H
