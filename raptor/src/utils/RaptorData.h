//
// Created by MichaelBrunner on 20/07/2024.
//

#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace raptor {

  struct RouteStop
  {
    int stopIndex;
    int routeIndex;
  };

  struct StopTime
  {
    int arrival;
    int departure;
  };

  struct Route
  {
    std::string id;
    int firstRouteStopIndex;
    int numberOfStops;
    int firstStopTimeIndex;
    int numberOfTrips;
    std::vector<std::string> tripIds;
  };

  struct Stop
  {
    std::string id;
    int stopRouteIndex;
    int numberOfRoutes;
    int sameStopTransferTime;
    int transferIndex;
    int numberOfTransfers;
  };

  struct Transfer
  {
    int targetStopIndex;
    int duration;
  };

  struct Lookup
  {
    std::unordered_map<std::string, int> stops;
    std::unordered_map<std::string, int> routes;
  };

  struct StopContext
  {
    std::vector<Transfer> transfers;
    std::vector<Stop> stops;
    std::vector<int> stopRoutes;
  };

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

  class RaptorData
  {
    Lookup lookup;
    StopContext stopContext;
    RouteTraversal routeTraversal;

  public:
    explicit RaptorData(Lookup  lookup, StopContext  stopContext, RouteTraversal  routeTraversal);
    const Lookup& getLookup() const;
    const StopContext& getStopContext() const;
    const RouteTraversal& getRouteTraversal() const;
  };

} // raptor
