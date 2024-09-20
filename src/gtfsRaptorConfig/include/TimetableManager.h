//
// Created by Michael Brunner on 13/06/2024.
//

#pragma once

#include "GtfsData.h"
#include "LocalDateTime.h"
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

    raptor::utils::LocalDateTime localDateTime;

    std::vector<schedule::gtfs::Trip*> activeTrips;

  public:
    explicit TimetableManager(schedule::gtfs::GtfsData&& data, const raptor::utils::LocalDateTime& localDateTime);

    [[nodiscard]] const schedule::gtfs::GtfsData& getData() const;

    [[nodiscard]] schedule::gtfs::GtfsData& getData();

    [[nodiscard]] RoutePartitioner& getRoutePartitioner();

    [[nodiscard]] const RoutePartitioner& getRoutePartitioner() const;

    [[nodiscard]] std::unordered_set<schedule::gtfs::Route*> getRoutes();

  private:
    void createRelations();
    void buildTripsToRoutesRelations() const;
    void buildStopTimesToTripsAndRoutesRelations() const;
    void buildStopRelations() const;
    void getActiveTrips(const raptor::utils::LocalDateTime& localDateTime);
    [[nodiscard]] bool isServiceAvailable(const std::string& serviceId, const raptor::utils::LocalDateTime& localDateTime) const;
    [[nodiscard]] std::unordered_set<schedule::gtfs::Route*> getRoutesForActiveTrips();
  };
} // gtfs
