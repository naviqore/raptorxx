//
// Created by MichaelBrunner on 13/06/2024.
//

#ifndef RELATIONMANAGER_H
#define RELATIONMANAGER_H

#include "GtfsData.h"
#include <vector>
#include "model/StopTime.h"
#include "model/Trip.h"
#include "model/Stop.h"
#include "model/Route.h"

#include <iostream>
#include <schedule_export.h>
#include <ranges>

namespace schedule::gtfs {

  class SCHEDULE_API RelationManager
  {
    GtfsData data;

  public:
    explicit RelationManager(GtfsData&& data);

    [[nodiscard]] const GtfsData& getData() const;

    [[nodiscard]] const std::string& getStopNameFromStopId(std::string const& aStopId) const;

    [[nodiscard]] const std::string& getStopIdFromStopName(std::string const& aStopName) const;

    [[nodiscard]] std::vector<std::string> getStopIdsFromStopName(std::string const& aStopName) const;

    [[nodiscard]] const StopTime& getStopTimeFromStopId(std::string const& aStopId) const;

    [[nodiscard]] std::vector<Stop> getAllStopsOnTrip(std::string const& aTripId) const;

    [[nodiscard]] const std::vector<StopTime>& getStopTimesFromStopId(std::string const& aStopId) const;

    [[nodiscard]] std::vector<Route> getRouteFromTripId(std::string const& aTripId) const;

    [[nodiscard]] std::vector<StopTime> getStopTimesFromStopIdStartingFromSpecificTime(std::string const& aStopId, unsigned int secondsGreaterThan) const;

    [[nodiscard]] bool isServiceActiveOnDay(std::string const& aServiceId, std::chrono::weekday aDay) const;

    [[nodiscard]] const std::vector<Trip>& getTripsFromStopTimeTripId(std::string const& aTripId) const;

  private:
    void createRelations();
    // // TODO reserve size for maps
    // // using stopTimeId = std::string;
    // using stopId = std::string;
    // using tripId = std::string;
    // using routeId = std::string;
    // using serviceId = std::string;
    //
    // // via Stop - stop_id we can get the StopTime
    // std::unordered_map<stopId, std::vector<StopTime>> stopTimeForStop;
    // // Transfer has two stop items from - to
    // std::unordered_map<stopId, std::vector<Transfer>> transfersForStop;
    // // via stopTime - trip_id we can get the Trip
    // std::unordered_map<tripId, std::vector<Trip>> tripsForRoute;
    // // via Trip - route_id we can get the Route
    // std::unordered_map<routeId, std::vector<Route>> routesForStop;
    // // via Trip - service_id we can get the calendar
    // std::unordered_map<serviceId, Calendar> calendarForTrip;
    // // via Calendar - service_id we can get the CalendarDates
    // std::unordered_map<serviceId, CalendarDate> calendarDatesForCalendar;
  };
} // gtfs

#endif //RELATIONMANAGER_H
