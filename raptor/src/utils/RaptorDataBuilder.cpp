//
// Created by MichaelBrunner on 20/07/2024.
//

#include "RaptorDataBuilder.h"
#include "RouteBuilder.h" // Assuming RouteBuilder is also converted and available
#include <algorithm>
#include <stdexcept>
#include <iostream> // For logging, replacing @Slf4j
#include <numeric>
#include <ranges>

namespace raptor {

  RaptorRouterBuilder::RaptorRouterBuilder(int defaultSameStopTransferTime)
    : defaultSameStopTransferTime(defaultSameStopTransferTime) {}

  RaptorRouterBuilder& RaptorRouterBuilder::addStop(const std::string& id) {
    if (stops.find(id) != stops.end())
    {
      throw std::invalid_argument("Stop " + id + " already exists");
    }
    std::cout << "Adding stop: id=" << id << std::endl;
    stops[id] = static_cast<int>(stops.size());
    stopRoutes[id] = std::unordered_set<std::string>();

    return *this;
  }

  RaptorRouterBuilder& RaptorRouterBuilder::addRoute(const std::string& id, const std::vector<std::string>& stopIds) {
    if (routeBuilders.contains(id))
    {
      throw std::invalid_argument("Route " + id + " already exists");
    }

    for (const auto& stopId : stopIds)
    {
      if (stops.find(stopId) == stops.end())
      {
        throw std::invalid_argument("Stop " + stopId + " does not exist");
      }
      stopRoutes[stopId].insert(id);
    }

    std::cout << "Adding route: id=" << id << ", stopSequence=[";
    for (const auto& stopId : stopIds)
    {
      std::cout << stopId << " ";
    }
    std::cout << "]" << std::endl;

    routeBuilders[id] = std::make_shared<RouteBuilder>(id, stopIds);
    routeStopSize += static_cast<int>(stopIds.size());

    return *this;
  }

  RaptorRouterBuilder& RaptorRouterBuilder::addTrip(const std::string& tripId, const std::string& routeId) {
    getRouteBuilder(routeId)->addTrip(tripId);
    return *this;
  }

  RaptorRouterBuilder& RaptorRouterBuilder::addStopTime(const std::string& routeId, const std::string& tripId, int position, const std::string& stopId, int arrival, int departure) {
    StopTime stopTime(arrival, departure);
    getRouteBuilder(routeId)->addStopTime(tripId, position, stopId, stopTime);
    stopTimeSize++;
    return *this;
  }

  RaptorRouterBuilder& RaptorRouterBuilder::addTransfer(const std::string& sourceStopId, const std::string& targetStopId, int duration) {
    std::cout << "Adding transfer: sourceStopId=" << sourceStopId << ", targetStopId=" << targetStopId << ", duration=" << duration << std::endl;

    if (stops.find(sourceStopId) == stops.end())
    {
      throw std::invalid_argument("Source stop " + sourceStopId + " does not exist");
    }

    if (stops.find(targetStopId) == stops.end())
    {
      throw std::invalid_argument("Target stop " + targetStopId + " does not exist");
    }

    if (sourceStopId == targetStopId)
    {
      sameStopTransfers[sourceStopId] = duration;
      return *this;
    }

    transfers[sourceStopId].emplace_back(stops[targetStopId], duration);
    transferSize++;
    return *this;
  }

  std::shared_ptr<RaptorData> RaptorRouterBuilder::build() {
    std::cout << "Initialize Raptor with " << stops.size() << " stops, " << routeBuilders.size() << " routes, " << routeStopSize << " route stops, " << stopTimeSize << " stop times, " << transferSize << " transfers" << std::endl;

    auto routeContainers = buildAndSortRouteContainers();
    auto lookup = buildLookup(routeContainers);
    auto stopContext = buildStopContext(lookup);
    auto routeTraversal = buildRouteTraversal(routeContainers);

    return std::make_shared<RaptorData>(lookup, stopContext, routeTraversal);
  }

  std::vector<RouteContainer> RaptorRouterBuilder::buildAndSortRouteContainers() {
    std::vector<RouteContainer> containers;
    for (auto& entry : routeBuilders)
    {
      containers.push_back(entry.second->build());
    }
    std::sort(containers.begin(), containers.end());
    return containers;
  }

  Lookup RaptorRouterBuilder::buildLookup(const std::vector<RouteContainer>& routeContainers) {
    std::cout << "Building lookup with " << stops.size() << " stops and " << routeContainers.size() << " routes" << std::endl;
    std::unordered_map<std::string, int> routes;
    for (size_t i = 0; i < routeContainers.size(); ++i)
    {
      routes[routeContainers[i].id()] = static_cast<int>(i);
    }
    return Lookup(stops, routes);
  }

  StopContext RaptorRouterBuilder::buildStopContext(const Lookup& lookup) {
    std::cout << "Building stop context with " << stops.size() << " stops and " << transferSize << " transfers" << std::endl;

    std::vector<Stop> stopArr(stops.size());
    auto stopRouteSizeView = stopRoutes
                             | std::views::values
                             | std::views::transform([](const std::unordered_set<std::string>& routes) { return static_cast<int>(routes.size()); });

    const auto stopRouteSize = std::accumulate(stopRouteSizeView.begin(), stopRouteSizeView.end(), 0);

    std::vector<int>
      stopRouteArr(stopRouteSize);
    std::vector<Transfer> transferArr(transferSize);

    int transferIdx = 0;
    int stopRouteIdx = 0;
    for (const auto& entry : stops)
    {
      const auto& stopId = entry.first;
      int stopIdx = entry.second;

      const auto& currentStopRoutes = stopRoutes[stopId];
      if (currentStopRoutes.empty())
      {
        throw std::logic_error("Stop " + stopId + " has no routes");
      }

      auto currentTransfers = transfers.find(stopId);
      auto numberOfTransfers = currentTransfers == transfers.end() ? 0 : currentTransfers->second.size();

      auto sameStopTransferTime = sameStopTransfers.count(stopId) ? sameStopTransfers[stopId] : defaultSameStopTransferTime;

      stopArr[stopIdx] = Stop(stopId, stopRouteIdx, static_cast<int>(currentStopRoutes.size()), sameStopTransferTime, numberOfTransfers == 0 ? NO_INDEX : transferIdx, static_cast<int>(numberOfTransfers));

      if (currentTransfers != transfers.end())
      {
        for (const auto& transfer : currentTransfers->second)
        {
          transferArr[transferIdx++] = transfer;
        }
      }

      for (const auto& routeId : currentStopRoutes)
      {
        stopRouteArr[stopRouteIdx++] = lookup.routes.at(routeId);
      }
    }

    return StopContext(transferArr, stopArr, stopRouteArr);
  }

  RouteTraversal RaptorRouterBuilder::buildRouteTraversal(const std::vector<RouteContainer>& routeContainers) {
    std::cout << "Building route traversal with " << routeContainers.size() << " routes, " << routeStopSize << " route stops, " << stopTimeSize << " stop times" << std::endl;

    std::vector<Route> routeArr(routeContainers.size());
    std::vector<RouteStop> routeStopArr(routeStopSize);
    std::vector<StopTime> stopTimeArr(stopTimeSize);

    int routeStopCnt = 0;
    int stopTimeCnt = 0;
    for (size_t routeIdx = 0; routeIdx < routeContainers.size(); ++routeIdx)
    {
      const auto& routeContainer = routeContainers[routeIdx];

      auto numberOfStops = routeContainer.stopSequence().size();
      auto numberOfTrips = routeContainer.trips().size();
      auto tripIdsArray = routeContainer.trips() | std::views::keys | std::ranges::to<std::vector<std::string>>();
      routeArr[routeIdx] = Route(routeContainer.id(), routeStopCnt, static_cast<int>(numberOfStops), stopTimeCnt, static_cast<int>(numberOfTrips), tripIdsArray);

      for (int position = 0; position < numberOfStops; ++position)
      {
        int stopIdx = stops.at(routeContainer.stopSequence().at(position));
        routeStopArr[routeStopCnt++] = RouteStop(stopIdx, static_cast<int>(routeIdx));
      }

      for (const auto& tripEntry : routeContainer.trips())
      {
        for (const auto& stopTime : tripEntry.second)
        {
          stopTimeArr[stopTimeCnt++] = stopTime;
        }
      }
    }

    return {routeStopArr, stopTimeArr, routeArr};
  }

  RouteBuilder* RaptorRouterBuilder::getRouteBuilder(const std::string& routeId) {
    auto it = routeBuilders.find(routeId);
    if (it == routeBuilders.end())
    {
      throw std::invalid_argument("Route " + routeId + " does not exist");
    }
    return it->second.get();
  }

} // namespace raptor