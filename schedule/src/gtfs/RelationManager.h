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
  };
} // gtfs

#endif //RELATIONMANAGER_H
