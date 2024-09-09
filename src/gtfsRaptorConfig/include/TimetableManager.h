//
// Created by Michael Brunner on 13/06/2024.
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

    [[nodiscard]] std::vector<schedule::gtfs::Route> getRoutes() const;

  private:
    void createRelations() const;
    void buildTripsToRoutesRelations() const;
    void buildStopTimesToTripsAndRoutesRelations() const;
    void buildStopTransferRelations() const;
    void buildParentChildrenStationRelations() const;
    void buildStopRelations() const;
  };
} // gtfs
