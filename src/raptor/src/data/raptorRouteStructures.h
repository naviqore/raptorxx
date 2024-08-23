//
// Created by MichaelBrunner on 26/07/2024.
//

#pragma once

#include "usingTypes.h"


#include <string>
#include <unordered_map>
#include <vector>

namespace raptor {

  struct RouteStop
  {
    types::raptorIdx stopIndex;
    types::raptorIdx routeIndex;
  };

  //////////////////////////////////////////////////////////////////////////

  struct StopTime
  {
    types::raptorInt arrival;
    types::raptorInt departure;
  };

  //////////////////////////////////////////////////////////////////////////

  struct Route
  {
    std::string id;
    types::raptorIdx firstRouteStopIndex;
    types::raptorInt numberOfStops;
    types::raptorIdx firstStopTimeIndex;
    types::raptorInt numberOfTrips;
    std::vector<std::string> tripIds;
  };

  //////////////////////////////////////////////////////////////////////////

  struct Stop
  {
    std::string id;
    types::raptorIdx stopRouteIndex;
    types::raptorInt numberOfRoutes;
    types::raptorInt sameStopTransferTime;
    int transferIndex;
    types::raptorInt numberOfTransfers;
  };

  //////////////////////////////////////////////////////////////////////////

  struct Transfer
  {
    types::raptorIdx targetStopIndex;
    types::raptorInt duration;
  };

  //////////////////////////////////////////////////////////////////////////

  struct StopRoutesIndexLookup
  {
    std::unordered_map<std::string, types::raptorIdx> stops;
    std::unordered_map<std::string, types::raptorIdx> routes;
  };

  //////////////////////////////////////////////////////////////////////////

  struct StopContext
  {
    std::vector<Transfer> transfers;
    std::vector<Stop> stops;
    std::vector<types::raptorIdx> stopRoutes;
  };

  //////////////////////////////////////////////////////////////////////////

  struct RouteTraversal
  {
    std::vector<RouteStop> routeStops;
    std::vector<StopTime> stopTimes;
    std::vector<Route> routes;

    RouteTraversal(std::vector<RouteStop> routeStops, std::vector<StopTime> stopTimes, std::vector<Route> routes)
      : routeStops(std::move(routeStops))
      , stopTimes(std::move(stopTimes))
      , routes(std::move(routes)) {
    }
  };
}