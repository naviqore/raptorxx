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

  namespace detail {
    inline auto SubRouteHash = [](const SubRoute& aSubRoute) {
      return std::hash<std::string>{}(aSubRoute.getSubRouteId());
    };

    inline auto SubRouteEqual = [](const SubRoute& aSubRoute, const SubRoute& anotherSubRoute) {
      return aSubRoute == anotherSubRoute;
    };
  }


  class GTFS_RAPTOR_API GtfsToRaptorConverter {
    std::unordered_set<std::string> addedStopIds{};
    raptor::RaptorRouterBuilder raptorRouterBuilder;
    std::unique_ptr<TimetableManager> timetableManager;
    std::unique_ptr<RoutePartitioner> routePartitioner;
    std::unordered_set<SubRoute, decltype(detail::SubRouteHash), decltype(detail::SubRouteEqual)> addedSubRoutes;

  public:
    explicit GtfsToRaptorConverter(int defaultSameStopTransferTime, std::unique_ptr<TimetableManager>&& timeTableManager);

    std::shared_ptr<raptor::RaptorData> convert();
    void processTrip(const std::shared_ptr<schedule::gtfs::Trip>& trip);
    void addSubRoute(const SubRoute& subRoute);

  private:
    void addStopTimesToRouterBuilder(schedule::gtfs::StopTime const& stopTime, std::string const& tripId, std::string const& subRouteId, int position);
    void addTransfers();
  };

} // converter
