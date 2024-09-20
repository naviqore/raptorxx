//
// Created by MichaelBrunner on 20/07/2024.
//

#include "RaptorRouterBuilder.h"

#include "LoggerFactory.h"
#include "RouteBuilder.h"
#include "helperFunctions.h"

#include <algorithm>
#include <execution>
#include <stdexcept>
#include <iostream>
#include <numeric>
#include <ranges>
#include <format>
#include <mutex>

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
#if LOGGER
    getConsoleLogger(LoggerName::RAPTOR)->info(std::format("Adding stop: id={}", id));
#endif

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
    const auto stopTime = StopTime{arrival, departure};
    getRouteBuilder(routeId)->addStopTime(tripId, position, stopId, stopTime);
    stopTimeSize++;
    return *this;
  }

  RaptorRouterBuilder& RaptorRouterBuilder::addTransfer(const std::string& sourceStopId, const std::string& targetStopId, int duration)
  {
#if LOGGER
    getConsoleLogger(LoggerName::RAPTOR)->info(std::format("Adding transfer: sourceStopId={} targetStopId={} duration={}", sourceStopId, targetStopId, duration));
#endif


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
#if LOGGER
    getConsoleLogger(LoggerName::RAPTOR)->info(std::format("Initialize Raptor with {} stops, {} routes, {} route stops, {} stop times, {} transfers", stops.size(), routeBuilders.size(), routeStopSize, stopTimeSize, transferSize));
#endif


    const auto routeContainers = buildAndSortRouteContainers();
    auto lookup = buildLookup(routeContainers);
    auto stopContext = buildStopContext(lookup);
    auto routeTraversal = buildRouteTraversal(routeContainers);

    return std::make_shared<RaptorData>(lookup, stopContext, routeTraversal);
  }

  std::vector<RouteContainer> RaptorRouterBuilder::buildAndSortRouteContainers() const
  {
    std::vector<RouteContainer> containers;
    containers.reserve(routeBuilders.size());
    for (const auto& routeBuilder : routeBuilders | std::views::values) {
      containers.push_back(routeBuilder->build());
    }

    std::ranges::sort(containers, [](const RouteContainer& lhs, const RouteContainer& rhs) {
      if (lhs.trips().empty()) {
        return false;
      }
      if (rhs.trips().empty()) {
        return true;
      }
      return lhs.trips().begin()->second[0].arrival < rhs.trips().begin()->second[0].arrival;
    });


    return containers;
  }

  StopRoutesIndexLookup RaptorRouterBuilder::buildLookup(const std::vector<RouteContainer>& routeContainers) const
  {
#if LOGGER
    getConsoleLogger(LoggerName::RAPTOR)->info(std::format("Building lookup with {} stops and {} routes", stops.size(), routeContainers.size()));
#endif

    std::unordered_map<std::string, types::raptorIdx> routes;
    for (auto i = 0; i < routeContainers.size(); ++i) {
      routes[routeContainers[i].id()] = i;
    }
    return {stops, routes};
  }

  StopContext RaptorRouterBuilder::buildStopContext(const StopRoutesIndexLookup& lookup)
  {
#if LOGGER
    getConsoleLogger(LoggerName::RAPTOR)->info(std::format("Building stop context with {} stops and {} transfers", stops.size(), transferSize));
#endif

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

    return {transferArr, stopArr, stopRouteArr};
  }

  RouteTraversal RaptorRouterBuilder::buildRouteTraversal(const std::vector<RouteContainer>& routeContainers) const
  {
#if LOGGER
    getConsoleLogger(LoggerName::RAPTOR)->info(std::format("Building route traversal with {} routes, {} route stops, {} stop times", routeContainers.size(), routeStopSize, stopTimeSize));
#endif

    std::vector<Route> routeArr(routeContainers.size());
    std::vector<RouteStop> routeStopArr(static_cast<types::raptorIdx>(routeStopSize));
    std::vector<StopTime> stopTimeArr(static_cast<types::raptorIdx>(stopTimeSize));

    types::raptorIdx routeStopCnt{0};
    types::raptorIdx stopTimeCnt{0};

    for (size_t routeIdx = 0; routeIdx < routeContainers.size(); ++routeIdx) {
      const auto& routeContainer = routeContainers[routeIdx];

      const auto& stopSequence = routeContainer.stopSequence();
      const auto& trips = routeContainer.trips();
      const auto numberOfStops = stopSequence.size();
      const auto numberOfTrips = trips.size();

      std::vector<std::string> tripIdsArray;
      tripIdsArray.reserve(routeContainer.trips().size());
      for (const auto& trip : routeContainer.trips()) {
        tripIdsArray.push_back(trip.first);
      }

      routeArr[routeIdx] = Route(routeContainer.id(), routeStopCnt, static_cast<int>(numberOfStops), stopTimeCnt, static_cast<int>(numberOfTrips), tripIdsArray);

      for (int position = 0; position < numberOfStops; ++position) {
        const auto stopIdxIt = stops.find(routeContainer.stopSequence().at(position));
        if (stopIdxIt == stops.end()) {
          throw std::invalid_argument("Stop " + routeContainer.stopSequence().at(position) + " does not exist");
        }
        routeStopArr[routeStopCnt++] = RouteStop(stopIdxIt->second, static_cast<types::raptorIdx>(routeIdx));
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


/*
 * //
// Created by MichaelBrunner on 20/07/2024.
//

#include "RaptorRouterBuilder.h"

#include "LoggerFactory.h"
#include "RouteBuilder.h"
#include "helperFunctions.h"

#include <algorithm>
#include <execution>
#include <stdexcept>
#include <iostream>
#include <numeric>
#include <ranges>
#include <format>
#include <mutex>

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
#if LOGGER
    getConsoleLogger(LoggerName::RAPTOR)->info(std::format("Adding stop: id={}", id));
#endif

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
    const auto stopTime = StopTime{arrival, departure};
    getRouteBuilder(routeId)->addStopTime(tripId, position, stopId, stopTime);
    stopTimeSize++;
    return *this;
  }

  RaptorRouterBuilder& RaptorRouterBuilder::addTransfer(const std::string& sourceStopId, const std::string& targetStopId, int duration)
  {
#if LOGGER
    getConsoleLogger(LoggerName::RAPTOR)->info(std::format("Adding transfer: sourceStopId={} targetStopId={} duration={}", sourceStopId, targetStopId, duration));
#endif


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
#if LOGGER
    getConsoleLogger(LoggerName::RAPTOR)->info(std::format("Initialize Raptor with {} stops, {} routes, {} route stops, {} stop times, {} transfers", stops.size(), routeBuilders.size(), routeStopSize, stopTimeSize, transferSize));
#endif


    const auto routeContainers = buildAndSortRouteContainers();
    auto lookup = buildLookup(routeContainers);
    auto stopContext = buildStopContext(lookup);
    auto routeTraversal = buildRouteTraversal(routeContainers);

    return std::make_shared<RaptorData>(lookup, stopContext, routeTraversal);
  }

  std::vector<RouteContainer> RaptorRouterBuilder::buildAndSortRouteContainers() const
  {
    std::vector<RouteContainer> containers;
    containers.reserve(routeBuilders.size());
    for (const auto& routeBuilder : routeBuilders | std::views::values) {
      containers.push_back(routeBuilder->build());
    }

    std::ranges::sort(containers, [](const RouteContainer& lhs, const RouteContainer& rhs) {
      if (lhs.trips().empty()) {
        return false;
      }
      if (rhs.trips().empty()) {
        return true;
      }
      return lhs.trips().begin()->second[0].arrival < rhs.trips().begin()->second[0].arrival;
    });


    return containers;
  }

  StopRoutesIndexLookup RaptorRouterBuilder::buildLookup(const std::vector<RouteContainer>& routeContainers) const
  {
#if LOGGER
    getConsoleLogger(LoggerName::RAPTOR)->info(std::format("Building lookup with {} stops and {} routes", stops.size(), routeContainers.size()));
#endif

    std::unordered_map<std::string, types::raptorIdx> routes;
    for (auto i = 0; i < routeContainers.size(); ++i) {
      routes[routeContainers[i].id()] = i;
    }
    return {stops, routes};
  }

  StopContext RaptorRouterBuilder::buildStopContext(const StopRoutesIndexLookup& lookup)
  {
#if LOGGER
    getConsoleLogger(LoggerName::RAPTOR)->info(std::format("Building stop context with {} stops and {} transfers", stops.size(), transferSize));
#endif

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

    return {transferArr, stopArr, stopRouteArr};
  }

  RouteTraversal RaptorRouterBuilder::buildRouteTraversal(const std::vector<RouteContainer>& routeContainers) const
  {
#if LOGGER
    getConsoleLogger(LoggerName::RAPTOR)->info(std::format("Building route traversal with {} routes, {} route stops, {} stop times", routeContainers.size(), routeStopSize, stopTimeSize));
#endif

    std::vector<Route> routeArr;
    routeArr.reserve(routeContainers.size());

    std::vector<RouteStop> routeStopArr;
    routeStopArr.reserve(static_cast<types::raptorIdx>(routeStopSize));

    std::vector<StopTime> stopTimeArr;
    stopTimeArr.reserve(static_cast<types::raptorIdx>(stopTimeSize));

    types::raptorIdx routeStopCnt{0};
    types::raptorIdx stopTimeCnt{0};

    for (size_t routeIdx = 0; routeIdx < routeContainers.size(); ++routeIdx) {
      const auto& routeContainer = routeContainers[routeIdx];

      const auto& stopSequence = routeContainer.stopSequence();
      const auto& trips = routeContainer.trips();
      const auto numberOfStops = stopSequence.size();
      const auto numberOfTrips = trips.size();

      std::vector<std::string> tripIdsArray;
      tripIdsArray.reserve(routeContainer.trips().size());
      for (const auto& trip : routeContainer.trips()) {
        tripIdsArray.push_back(trip.first);
      }

      // routeArr[routeIdx] = Route(routeContainer.id(), routeStopCnt, static_cast<int>(numberOfStops), stopTimeCnt, static_cast<int>(numberOfTrips), tripIdsArray);
      routeArr.emplace_back(routeContainer.id(), routeStopCnt, static_cast<int>(numberOfStops), stopTimeCnt, static_cast<int>(numberOfTrips), tripIdsArray);

      for (int position = 0; position < numberOfStops; ++position) {
        const auto stopIdxIt = stops.find(routeContainer.stopSequence().at(position));
        if (stopIdxIt == stops.end()) {
          throw std::invalid_argument("Stop " + routeContainer.stopSequence().at(position) + " does not exist");
        }
        // routeStopArr[routeStopCnt++] = RouteStop(stopIdxIt->second, static_cast<types::raptorIdx>(routeIdx));
        routeStopArr.emplace_back(stopIdxIt->second, static_cast<types::raptorIdx>(routeIdx));
        routeStopCnt++;
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

 */
