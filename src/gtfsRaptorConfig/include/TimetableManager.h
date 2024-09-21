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

  public:
    explicit TimetableManager(schedule::gtfs::GtfsData&& data, const raptor::utils::LocalDateTime& localDateTime);

    [[nodiscard]] const schedule::gtfs::GtfsData& getData() const;

    [[nodiscard]] schedule::gtfs::GtfsData& getData();

    [[nodiscard]] RoutePartitioner& getRoutePartitioner();

    [[nodiscard]] const RoutePartitioner& getRoutePartitioner() const;

  private:
    void createRelations() const;
    void buildTripsToRoutesRelations() const;
    void buildStopTimesToTripsAndRoutesRelations() const;
    void buildStopRelations() const;
    void markActiveTrips(const raptor::utils::LocalDateTime& localDateTime) const;
    [[nodiscard]] bool isServiceAvailable(const std::string& serviceId, const raptor::utils::LocalDateTime& localDateTime) const;
  };
} // gtfs
