//
// Created by MichaelBrunner on 27/06/2024.
//

#include "RaptorStrategy.h"
#include <LoggingPool.h>
#include <format>
#include <utility>
#include "raptorTypes.h"

namespace raptor::strategy {

  RaptorStrategy::RaptorStrategy(schedule::gtfs::RelationManager&& relationManager)
    : relationManager(std::move(relationManager)) {
  }
  std::shared_ptr<IConnection> RaptorStrategy::execute(const utils::ConnectionRequest& request) {

    LoggingPool::getInstance(Target::CONSOLE)->info(std::format("Agency: {} {} {}", request.departureStop.stopName, request.departureTime, request.arrivalStop.stopName));

    std::vector<int> arrivalTimes;
    std::vector<int> bestTimes;

    struct RouteStop
    {
      int routeId;
      int stopId;
    };

    std::vector<bool> visited;

    /*arrivalTimes.assign(relationManager.getStopsForRoute(request.arrivalStop.stopId).size(), utils::INFINITY_VALUE);
    bestTimes.assign(relationManager.getStopsForRoute(request.arrivalStop.stopId).size(), utils::INFINITY_VALUE);

    const auto sourceStops = relationManager.getStopsForRoute(request.arrivalStop.stopId);
    const auto targetStops = relationManager.getStopsForRoute(request.departureStop.stopId);*/

    // arrivalTimes[request.arrivalStop.stopId] = request.departureTime;


    return nullptr;
  }
} // strategy
  // gtfs
  // raptor