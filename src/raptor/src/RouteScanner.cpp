//
// Created by MichaelBrunner on 20/07/2024.
//

#include "RouteScanner.h"

#include "LoggerFactory.h"
#include "data/ActiveTrip.h"

#include <iostream>
#include <format>

namespace raptor {

  bool canEnterAtStop(const Stop& stop, const types::raptorInt stopTime, const std::unordered_set<types::raptorIdx>& markedStops, const types::raptorIdx stopIdx, const types::raptorInt stopOffset, const types::raptorInt numberOfStops)
  {

    if (constexpr auto unreachableValue = types::INFINITY_VALUE_MAX;
        stopTime == unreachableValue) {
#if LOGGER
      getConsoleLogger(LoggerName::RAPTOR)->info(std::format("Stop {} cannot be reached", stop.id));
#endif

      return false;
    }

    if (!markedStops.contains(stopIdx)) {
#if LOGGER
      getConsoleLogger(LoggerName::RAPTOR)->info(std::format("Stop {} was not improved in previous round, continue", stop.id));
#endif

      return false;
    }

    if (stopOffset + 1 == numberOfStops) {
#if LOGGER
      getConsoleLogger(LoggerName::RAPTOR)->info(std::format("Stop {} is last stop in route, continue", stop.id));
#endif

      return false;
    }
#if LOGGER
    getConsoleLogger(LoggerName::RAPTOR)->info(std::format("Stop {} is first stop in route, continue", stop.id));
#endif


    return true;
  }

  RouteScanner::RouteScanner(StopLabelsAndTimes& stopLabelsAndTimes, const RaptorData& raptorData, const types::raptorInt minimumTransferDuration)
    : stops(raptorData.getStopContext().stops)
    , stopRoutes(raptorData.getStopContext().stopRoutes)
    , stopTimes(raptorData.getRouteTraversal().stopTimes)
    , routes(raptorData.getRouteTraversal().routes)
    , routeStops(raptorData.getRouteTraversal().routeStops)
    , stopLabelsAndTimes(const_cast<StopLabelsAndTimes&>(stopLabelsAndTimes))
    , minTransferDuration(minimumTransferDuration)
    , routesToScanMask(this->routes.size(), false)
  {
  }

  void RouteScanner::scan(const int round)
  {
#if LOGGER
    getConsoleLogger(LoggerName::RAPTOR)->info(std::format("Scanning routes for round {} ({} routes)", round, this->routesToScanMask.size()));
#endif
    setRoutesToScan();

    for (auto currentRouteIndex{0}; currentRouteIndex < routesToScanMask.size(); currentRouteIndex++) {
      if (routesToScanMask[currentRouteIndex]) {
        scanRoute(currentRouteIndex, round);
      }
    }
  }


  std::unordered_set<types::raptorIdx> RouteScanner::getRoutesToScan(const std::vector<bool>& markedStops) const
  {
    std::unordered_set<types::raptorIdx> routesToScan(stopRoutes.size());

    for (size_t stopIdx = 0; stopIdx < markedStops.size(); ++stopIdx) {
      if (markedStops[stopIdx]) {
        const Stop& currentStop = stops[stopIdx];
        auto stopRouteIdx = currentStop.stopRouteIndex;
        const auto stopRouteEndIdx = stopRouteIdx + currentStop.numberOfRoutes;

        while (stopRouteIdx < stopRouteEndIdx) {
          routesToScan.insert(stopRoutes[stopRouteIdx]);
          ++stopRouteIdx;
        }
      }
    }
    return routesToScan;
  }

  void RouteScanner::scanRoute(const types::raptorIdx currentRouteIdx, const types::raptorInt round) const
  {
    const auto lastRound = round - 1;

    const Route& currentRoute = routes[currentRouteIdx];
#if LOGGER
    getConsoleLogger(LoggerName::RAPTOR)->info(std::format("Scanning route {}", currentRoute.id));
#endif

    const auto firstRouteStopIdx = currentRoute.firstRouteStopIndex;
    const auto firstStopTimeIdx = currentRoute.firstStopTimeIndex;
    const auto numberOfStops = currentRoute.numberOfStops;

    std::optional<ActiveTrip> activeTrip = std::nullopt;

    for (auto stopOffset = 0u; stopOffset < numberOfStops; ++stopOffset) {
      const auto stopIdx = routeStops[firstRouteStopIdx + stopOffset].stopIndex;
      const Stop& stop = stops[stopIdx];
      const auto bestStopTime = stopLabelsAndTimes.getComparableBestTime(stopIdx);

      if (!activeTrip) {
        // replace this "unclean code" with canEnterAtStop (performance...)
        if (bestStopTime == types::INFINITY_VALUE_MAX || !stopLabelsAndTimes.isMarkedThisRound(stopIdx) || stopOffset + 1 == numberOfStops) {
#if LOGGER
          getConsoleLogger(LoggerName::RAPTOR)->info(std::format("Stop {} cannot be entered", stop.id));
#endif
          continue;
        }
      }
      else {
        const StopTime& stopTimeObj = stopTimes[firstStopTimeIdx + activeTrip->tripOffset * numberOfStops + stopOffset];
        if (!checkIfTripIsPossibleAndUpdateMarks(stopTimeObj, activeTrip.value(), stop, bestStopTime, stopIdx, round, lastRound, currentRouteIdx)) {
          continue;
        }
      }

      activeTrip = findPossibleTrip(stopIdx, stop, stopOffset, currentRoute, lastRound);
    }
  }

  bool RouteScanner::checkIfTripIsPossibleAndUpdateMarks(const StopTime& stopTime, const ActiveTrip& activeTrip, const Stop& stop, const types::raptorInt bestStopTime, types::raptorIdx stopIdx, const types::raptorInt thisRound, const types::raptorInt lastRound, types::raptorIdx currentRouteIdx) const
  {

    if (const bool isImproved = stopTime.arrival < bestStopTime) {
#if LOGGER
      getConsoleLogger(LoggerName::RAPTOR)->info(std::format("Stop {} was improved", stop.id));
#endif

      stopLabelsAndTimes.setBestTime(stopIdx, stopTime.arrival);

      auto label = std::make_unique<StopLabelsAndTimes::Label>(activeTrip.entryTime,
                                                               stopTime.arrival,
                                                               StopLabelsAndTimes::LabelType::ROUTE,
                                                               currentRouteIdx,
                                                               activeTrip.tripOffset,
                                                               stopIdx,
                                                               activeTrip.previousLabel);

      stopLabelsAndTimes.setLabel(thisRound, stopIdx, std::move(label));
      stopLabelsAndTimes.mark(stopIdx);

      return false;
    }

#if LOGGER
    getConsoleLogger(LoggerName::RAPTOR)->info(std::format("Stop {} was not improved", stop.id));
#endif

    const auto previous = stopLabelsAndTimes.getLabel(lastRound, stopIdx);

    if (const bool isImprovedInSameRound = previous == nullptr || previous->targetTime >= stopTime.arrival) {
#if LOGGER
      getConsoleLogger(LoggerName::RAPTOR)->info(std::format("Stop {} has been improved in same round, trip not possible within this round", stop.id));
#endif

      return false;
    }
#if LOGGER
    getConsoleLogger(LoggerName::RAPTOR)->info(std::format("Stop {} has not been improved in same round, checking for trips", stop.id));
#endif

    return true;
  }

  std::optional<ActiveTrip> RouteScanner::findPossibleTrip(const types::raptorIdx stopIdx, const Stop& stop, const types::raptorInt stopOffset, const Route& route, const types::raptorInt lastRound) const
  {
    const auto firstStopTimeIdx = route.firstStopTimeIndex;
    const auto numberOfStops = route.numberOfStops;
    const auto numberOfTrips = route.numberOfTrips;

    types::raptorInt tripOffset = 0;
    auto previousLabel = stopLabelsAndTimes.getLabel(lastRound, stopIdx);

    auto referenceTime = previousLabel->targetTime;
    if (previousLabel->type == StopLabelsAndTimes::LabelType::ROUTE) {
      referenceTime += std::max(stop.sameStopTransferTime, minTransferDuration);
    }

    while (tripOffset < numberOfTrips) {
      if (const StopTime& currentStopTime = stopTimes[firstStopTimeIdx + tripOffset * numberOfStops + stopOffset];
          currentStopTime.departure >= referenceTime) {
#if LOGGER
        getConsoleLogger(LoggerName::RAPTOR)->info(std::format("Found active trip ({}) on route {}", tripOffset, route.id));
#endif

        types::raptorInt entryTime = currentStopTime.departure;
        return ActiveTrip(tripOffset, entryTime, previousLabel);
      }
      if (tripOffset < numberOfTrips - 1) {
        tripOffset += 1;
      }
      else {
#if LOGGER
        getConsoleLogger(LoggerName::RAPTOR)->info(std::format("No active trip found on route {}", route.id));
#endif

        return std::nullopt;
      }
    }

    return std::nullopt;
  }
  void RouteScanner::setRoutesToScan()
  {

    for (auto stopIdx{0}; stopIdx < stops.size(); stopIdx++) {
      if (false == stopLabelsAndTimes.isMarkedThisRound(stopIdx)) {
        continue;
      }
      const auto& currentStop = stops[stopIdx];
      auto stopRouteIdx = currentStop.stopRouteIndex;
      const auto stopRouteEndIdx = stopRouteIdx + currentStop.numberOfRoutes;
      while (stopRouteIdx < stopRouteEndIdx) {
        routesToScanMask[stopRoutes[stopRouteIdx]] = true;
        stopRouteIdx++;
      }
    }
  }
} // raptor
