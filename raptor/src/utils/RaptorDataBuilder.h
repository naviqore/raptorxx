//
// Created by MichaelBrunner on 20/07/2024.
//

#pragma once

#include "RaptorData.h"
#include "RouteBuilder.h"


#include <string>
#include <unordered_map>
#include <vector>
#include <set>
#include <memory>
#include <unordered_set>
#include <raptor_export.h>
// #include "RaptorAlgorithm.h" // Assuming RaptorAlgorithm is already converted to C++

namespace raptor {

  class RAPTOR_API RaptorRouterBuilder
  {
  public:
    explicit RaptorRouterBuilder(int defaultSameStopTransferTime);

    RaptorRouterBuilder& addStop(const std::string& id);
    RaptorRouterBuilder& addRoute(const std::string& id, const std::vector<std::string>& stopIds);
    RaptorRouterBuilder& addTrip(const std::string& tripId, const std::string& routeId);
    RaptorRouterBuilder& addStopTime(const std::string& routeId, const std::string& tripId, int position, const std::string& stopId, int arrival, int departure);
    RaptorRouterBuilder& addTransfer(const std::string& sourceStopId, const std::string& targetStopId, int duration);

    std::shared_ptr<RaptorData> build();

  private:
    int defaultSameStopTransferTime;
    std::unordered_map<std::string, int> stops;
    std::unordered_map<std::string, std::shared_ptr<RouteBuilder>> routeBuilders;
    std::unordered_map<std::string, std::vector<Transfer>> transfers;
    std::unordered_map<std::string, int> sameStopTransfers;
    std::unordered_map<std::string, std::unordered_set<std::string>> stopRoutes;

    int stopTimeSize = 0;
    int routeStopSize = 0;
    int transferSize = 0;
    int NO_INDEX{-1};

    std::vector<RouteContainer> buildAndSortRouteContainers();
    Lookup buildLookup(const std::vector<RouteContainer>& routeContainers);
    StopContext buildStopContext(const Lookup& lookup);
    RouteTraversal buildRouteTraversal(const std::vector<RouteContainer>& routeContainers);
    RouteBuilder* getRouteBuilder(const std::string& routeId);
  };


} // namespace raptor