//
// Created by MichaelBrunner on 13/06/2024.
//

#pragma once

#include "GtfsData.h"
#include "RoutePartitioner.h"
#include <vector>
#include "model/StopTime.h"
#include "model/Stop.h"
#include "model/Route.h"
#include <gtfsRaptorConfig_export.h>
#include <ranges>

namespace converter {

  class GTFS_RAPTOR_API TimetableManager {
    std::unique_ptr<schedule::gtfs::GtfsData> data;
    std::unique_ptr<RoutePartitioner> routePartitioner;

  public:
    explicit TimetableManager(schedule::gtfs::GtfsData&& data);

    [[nodiscard]] const schedule::gtfs::GtfsData& getData() const;

    [[nodiscard]] schedule::gtfs::GtfsData& getData();

    [[nodiscard]] RoutePartitioner& getRoutePartitioner();

    [[nodiscard]] const RoutePartitioner& getRoutePartitioner() const;

    [[nodiscard]] const std::string& getStopNameFromStopId(std::string const& aStopId) const;

    [[nodiscard]] const std::string& getStopIdFromStopName(std::string const& aStopName) const;

    [[nodiscard]] std::vector<std::string> getStopIdsFromStopName(std::string const& aStopName) const;

    [[nodiscard]] const schedule::gtfs::StopTime& getStopTimeFromStopId(std::string const& aStopId) const;

    [[nodiscard]] std::vector<schedule::gtfs::Stop> getAllStopsOnTrip(std::string const& aTripId) const;

    [[nodiscard]] const std::vector<schedule::gtfs::StopTime>& getStopTimesFromStopId(std::string const& aStopId) const;

    [[nodiscard]] schedule::gtfs::Route getRouteFromTripId(std::string const& aTripId) const;

    [[nodiscard]] std::vector<schedule::gtfs::StopTime> getStopTimesFromStopIdStartingFromSpecificTime(std::string const& aStopId, unsigned int secondsGreaterThan) const;

    [[nodiscard]] bool isServiceActiveOnDay(std::string const& aServiceId, std::chrono::weekday aDay) const;

    [[nodiscard]] const schedule::gtfs::Trip& getTripsFromStopTimeTripId(std::string const& aTripId) const;

    [[nodiscard]] std::vector<std::string> getVisitedStopIds() const;

  private:
    void createRelations() const;
    void buildTripsToRoutesRelations() const;
    void buildStopTimesToTripsAndRoutesRelations() const;
  };
} // gtfs
