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
#include "model/stopTime.h"


#include <vector>
#include <schedule_export.h>

namespace gtfs {

  struct GTFS_API GtfsData
  {
    std::vector<Agency> agencies;
    std::vector<Calendar> calendars;
    std::vector<CalendarDate> calendarDates;
    std::vector<Route> routes;
    std::vector<Stop> stops;
    std::vector<StopTime> stopTimes;
    std::vector<Transfer> transfers;
    std::vector<Trip> trips;
  };

} // gtfs

#endif //GTFSDATA_H
