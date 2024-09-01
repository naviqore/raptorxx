//
// Created by MichaelBrunner on 26/07/2024.
//

#pragma once

#include "RaptorRouter.h"
#include "RoutePartitioner.h"
#include "TimetableManager.h"
#include "RaptorRouterBuilder.h"


#include <gtfsRaptorConfig_export.h>
#include <unordered_set>
#include <string>

namespace converter {

  class GTFS_RAPTOR_API GtfsToRaptorConverter
  {
    std::unordered_set<std::string> addedStopIds{};
    raptor::RaptorRouterBuilder raptorRouterBuilder;
    TimetableManager timetableManager;
    std::unique_ptr<RoutePartitioner> routePartitioner;

  public:
    explicit GtfsToRaptorConverter(schedule::gtfs::GtfsData&& data, int defaultSameStopTransferTime);

    std::shared_ptr<raptor::RaptorData> convert();

  private:
    void addRoute(SubRoute const& subRoute);
    void addStopIdsToRoute(std::string const& subRouteId, const std::vector<std::string>& stopIdsVector);
    void addTripsToRouterBuilder(SubRoute const& subRoute);
    void addStopTimesToRouterBuilder(schedule::gtfs::StopTime const& stopTime, std::string const& tripId, std::string const& subRouteId, int position);
    void addTransfers();

  };

} // converter
