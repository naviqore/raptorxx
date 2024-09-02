//
// Created by MichaelBrunner on 26/07/2024.
//

#pragma once

#include "usingTypes.h"


#include <string>
#include <unordered_map>
#include <vector>
#include <raptor_export.h>

namespace raptor {

  struct RAPTOR_API RouteStop
  {
    types::raptorIdx stopIndex;
    types::raptorIdx routeIndex;
  };

  //////////////////////////////////////////////////////////////////////////

  struct RAPTOR_API StopTime
  {
    types::raptorInt arrival;
    types::raptorInt departure;
  };

  //////////////////////////////////////////////////////////////////////////

  struct RAPTOR_API Route
  {
    std::string id;
    types::raptorIdx firstRouteStopIndex{};
    types::raptorInt numberOfStops{};
    types::raptorIdx firstStopTimeIndex{};
    types::raptorInt numberOfTrips{};
    std::vector<std::string> tripIds;
  };

  //////////////////////////////////////////////////////////////////////////

  struct RAPTOR_API Stop
  {
    std::string id;
    types::raptorIdx stopRouteIndex{};
    types::raptorInt numberOfRoutes{};
    types::raptorInt sameStopTransferTime{};
    int transferIndex{};
    types::raptorInt numberOfTransfers{};
  };

  //////////////////////////////////////////////////////////////////////////

  struct RAPTOR_API Transfer
  {
    types::raptorIdx targetStopIndex;
    types::raptorInt duration;
  };

  //////////////////////////////////////////////////////////////////////////

  struct RAPTOR_API StopRoutesIndexLookup
  {
    std::unordered_map<std::string, types::raptorIdx> stops;
    std::unordered_map<std::string, types::raptorIdx> routes;
  };

  //////////////////////////////////////////////////////////////////////////

  struct RAPTOR_API StopContext
  {
    std::vector<Transfer> transfers;
    std::vector<Stop> stops;
    std::vector<types::raptorIdx> stopRoutes;
  };

  //////////////////////////////////////////////////////////////////////////

  struct RAPTOR_API RouteTraversal
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