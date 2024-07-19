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
    std::unordered_map<std::string, Agency> agencies;
    std::unordered_map<std::string, Calendar> calendars;
    std::unordered_map<std::string, std::vector<CalendarDate>> calendarDates;
    std::unordered_map<std::string, Route> routes;
    std::unordered_map<std::string, Stop> stops;
    std::unordered_map<std::string, std::vector<StopTime>> stopTimes;
    std::unordered_map<std::string, std::vector<Transfer>> transferFrom;
    std::unordered_map<std::string, std::vector<Transfer>> transferTo;
    std::unordered_map<std::string, Trip> trips;
  };

} // gtfs

#endif //GTFSDATA_H
