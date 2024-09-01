//
// Created by MichaelBrunner on 20/07/2024.
//

#include "RaptorRouterBuilder.h"

#include "LoggerFactory.h"
#include "RouteBuilder.h"
#include "helperFunctions.h"

#include <algorithm>
#include <stdexcept>
#include <iostream>
#include <numeric>
#include <ranges>
#include <format>

namespace raptor {

  RaptorRouterBuilder::RaptorRouterBuilder(const int defaultSameStopTransferTime)
    : defaultSameStopTransferTime(defaultSameStopTransferTime)
  {
  }

  RaptorRouterBuilder& RaptorRouterBuilder::addStop(const std::string& id)
  {
    if (stops.contains(id)) {
      throw std::invalid_argument("Stop " + id + " already exists");
    }
    getConsoleLogger(LoggerName::RAPTOR)->info(std::format("Adding stop: id={}", id));
    stops[id] = static_cast<types::raptorIdx>(stops.size());
    stopRoutes[id] = std::unordered_set<std::string>();

    return *this;
  }

  RaptorRouterBuilder& RaptorRouterBuilder::addRoute(const std::string& routeId, const std::vector<std::string>& stopIds)
  {
    if (routeBuilders.contains(routeId)) {
      throw std::invalid_argument("Route " + routeId + " already exists");
    }

    for (const auto& stopId : stopIds) {
      if (!stops.contains(stopId)) {
        throw std::invalid_argument("Stop " + stopId + " does not exist");
      }
      stopRoutes[stopId].insert(routeId);
    }

    const auto joinedStopIds = stopIds | std::views::join;
    // getConsoleLogger(LoggerName::RAPTOR)->info(std::format("Adding route: id={} stopSequence=[{}] ", routeId, utils::joinToString(joinedStopIds)));

    routeBuilders[routeId] = std::make_shared<RouteBuilder>(routeId, stopIds);
    routeStopSize += static_cast<int>(stopIds.size());

    return *this;
  }

  RaptorRouterBuilder& RaptorRouterBuilder::addTrip(const std::string& tripId, const std::string& routeId)
  {
    getRouteBuilder(routeId)->addTrip(tripId);
    return *this;
  }

  RaptorRouterBuilder& RaptorRouterBuilder::addStopTime(const std::string& routeId, const std::string& tripId, const int position, const std::string& stopId, types::raptorInt arrival, types::raptorInt departure)
  {
    const StopTime stopTime(arrival, departure);
    getRouteBuilder(routeId)->addStopTime(tripId, position, stopId, stopTime);
    stopTimeSize++;
    return *this;
  }

  RaptorRouterBuilder& RaptorRouterBuilder::addTransfer(const std::string& sourceStopId, const std::string& targetStopId, int duration)
  {
    getConsoleLogger(LoggerName::RAPTOR)->info(std::format("Adding transfer: sourceStopId={} targetStopId={} duration={}", sourceStopId, targetStopId, duration));

    if (!stops.contains(sourceStopId)) {
      throw std::invalid_argument("Source stop " + sourceStopId + " does not exist");
    }

    if (!stops.contains(targetStopId)) {
      throw std::invalid_argument("Target stop " + targetStopId + " does not exist");
    }

    if (sourceStopId == targetStopId) {
      sameStopTransfers[sourceStopId] = duration;
      return *this;
    }

    transfers[sourceStopId].emplace_back(stops[targetStopId], duration);
    transferSize++;
    return *this;
  }

  std::shared_ptr<RaptorData> RaptorRouterBuilder::build()
  {
    getConsoleLogger(LoggerName::RAPTOR)->info(std::format("Initialize Raptor with {} stops, {} routes, {} route stops, {} stop times, {} transfers", stops.size(), routeBuilders.size(), routeStopSize, stopTimeSize, transferSize));

    const auto routeContainers = buildAndSortRouteContainers();
    auto lookup = buildLookup(routeContainers);
    auto stopContext = buildStopContext(lookup);
    auto routeTraversal = buildRouteTraversal(routeContainers);

    return std::make_shared<RaptorData>(lookup, stopContext, routeTraversal);
  }

  std::vector<RouteContainer> RaptorRouterBuilder::buildAndSortRouteContainers() const
  {
    std::vector<RouteContainer> containers;
    for (const auto& routeBuilder : routeBuilders | std::views::values) {
      containers.push_back(routeBuilder->build());
    }
    std::sort(containers.begin(), containers.end());

    return containers;
  }

  StopRoutesIndexLookup RaptorRouterBuilder::buildLookup(const std::vector<RouteContainer>& routeContainers) const
  {
    getConsoleLogger(LoggerName::RAPTOR)->info(std::format("Building lookup with {} stops and {} routes", stops.size(), routeContainers.size()));
    std::unordered_map<std::string, types::raptorIdx> routes;
    for (auto i = 0; i < routeContainers.size(); ++i) {
      routes[routeContainers[i].id()] = i;
    }
    return StopRoutesIndexLookup(stops, routes);
  }

  StopContext RaptorRouterBuilder::buildStopContext(const StopRoutesIndexLookup& lookup)
  {
    getConsoleLogger(LoggerName::RAPTOR)->info(std::format("Building stop context with {} stops and {} transfers", stops.size(), transferSize));
    std::vector<Stop> stopArr(stops.size());
    auto stopRouteSizeView = stopRoutes
                             | std::views::values
                             | std::views::transform([](const std::unordered_set<std::string>& routes) { return static_cast<int>(routes.size()); });

    const auto stopRouteSize = std::accumulate(stopRouteSizeView.begin(), stopRouteSizeView.end(), 0);

    std::vector<types::raptorIdx> stopRouteArr(stopRouteSize);
    std::vector<Transfer> transferArr(transferSize);

    int transferIdx = 0;
    int stopRouteIdx = 0;
    for (const auto& [stopId, stopIdx] : stops) {
      const auto& currentStopRoutes = stopRoutes[stopId];
      if (currentStopRoutes.empty()) {
        throw std::logic_error("Stop " + stopId + " has no routes");
      }

      auto currentTransfers = transfers.find(stopId);
      const auto numberOfTransfers = currentTransfers == transfers.end() ? 0 : currentTransfers->second.size();

      const auto sameStopTransferTime = sameStopTransfers.contains(stopId) ? sameStopTransfers[stopId] : defaultSameStopTransferTime;

      stopArr[stopIdx] = Stop(stopId, stopRouteIdx, static_cast<int>(currentStopRoutes.size()), sameStopTransferTime, static_cast<int>(numberOfTransfers) == 0 ? NO_INDEX : transferIdx, static_cast<int>(numberOfTransfers));

      if (currentTransfers != transfers.end()) {
        for (const auto& transfer : currentTransfers->second) {
          transferArr[transferIdx++] = transfer;
        }
      }

      for (const auto& routeId : currentStopRoutes) {
        stopRouteArr[stopRouteIdx++] = lookup.routes.at(routeId);
      }
    }

    return StopContext(transferArr, stopArr, stopRouteArr);
  }

  RouteTraversal RaptorRouterBuilder::buildRouteTraversal(const std::vector<RouteContainer>& routeContainers) const
  {
    getConsoleLogger(LoggerName::RAPTOR)->info(std::format("Building route traversal with {} routes, {} route stops, {} stop times", routeContainers.size(), routeStopSize, stopTimeSize));
    std::vector<Route> routeArr(routeContainers.size());
    std::vector<RouteStop> routeStopArr(static_cast<types::raptorIdx>(routeStopSize));
    std::vector<StopTime> stopTimeArr(static_cast<types::raptorIdx>(stopTimeSize));

    types::raptorIdx routeStopCnt = 0;
    types::raptorIdx stopTimeCnt = 0;
    for (size_t routeIdx = 0; routeIdx < routeContainers.size(); ++routeIdx) {
      const auto& routeContainer = routeContainers[routeIdx];

      const auto numberOfStops = routeContainer.stopSequence().size();
      const auto numberOfTrips = routeContainer.trips().size();

#if defined(__cpp_lib_ranges_to_container)
      const auto tripIdsArray = routeContainer.trips() | std::views::keys | std::ranges::to<std::vector<std::string>>();
#else
      std::vector<std::string> tripIdsArray;
      std::ranges::transform(routeContainer.trips(), std::back_inserter(tripIdsArray), [](const auto& pair) { return pair.first; });
#endif
      routeArr[routeIdx] = Route(routeContainer.id(), routeStopCnt, static_cast<int>(numberOfStops), stopTimeCnt, static_cast<int>(numberOfTrips), tripIdsArray);

      for (int position = 0; position < numberOfStops; ++position) {
        const auto stopIdx = stops.at(routeContainer.stopSequence().at(position));
        routeStopArr[routeStopCnt++] = RouteStop(stopIdx, static_cast<types::raptorIdx>(routeIdx));
      }

      for (const auto& stopTimes : routeContainer.trips() | std::views::values) {
        for (const auto& stopTime : stopTimes) {
          stopTimeArr[stopTimeCnt++] = stopTime;
        }
      }
    }

    return {routeStopArr, stopTimeArr, routeArr};
  }

  RouteBuilder* RaptorRouterBuilder::getRouteBuilder(const std::string& routeId)
  {
    const auto it = routeBuilders.find(routeId);
    if (it == routeBuilders.end()) {
      throw std::invalid_argument("Route " + routeId + " does not exist");
    }
    return it->second.get();
  }

} // namespace raptor