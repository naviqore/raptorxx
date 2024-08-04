//
// Created by MichaelBrunner on 20/07/2024.
//

#include "RouteScanner.h"

#include "LoggerFactory.h"
#include "data/ActiveTrip.h"

#include <iostream>

namespace raptor {

  bool canEnterAtStop(const Stop& stop, const types::raptorInt stopTime, const std::unordered_set<types::raptorIdx>& markedStops, const types::raptorIdx stopIdx, const types::raptorInt stopOffset, const types::raptorInt numberOfStops) {

    if (constexpr auto unreachableValue = types::INFINITY_VALUE_MAX;
        stopTime == unreachableValue)
    {
      getConsoleLogger(LoggerName::RAPTOR)->info(std::format("Stop {} cannot be reached", stop.id));
      return false;
    }

    if (!markedStops.contains(stopIdx))
    {
      getConsoleLogger(LoggerName::RAPTOR)->info(std::format("Stop {} was not improved in previous round, continue", stop.id));
      return false;
    }

    if (stopOffset + 1 == numberOfStops)
    {
      getConsoleLogger(LoggerName::RAPTOR)->info(std::format("Stop {} is last stop in route, continue", stop.id));
      return false;
    }
    getConsoleLogger(LoggerName::RAPTOR)->info(std::format("Stop {} is first stop in route, continue", stop.id));

    return true;
  }

  RouteScanner::RouteScanner(StopLabelsAndTimes& stopLabelsAndTimes, const RaptorData& raptorData, const types::raptorInt minimumTransferDuration)
    : stops(raptorData.getStopContext().stops)
    , stopRoutes(raptorData.getStopContext().stopRoutes)
    , stopTimes(raptorData.getRouteTraversal().stopTimes)
    , routes(raptorData.getRouteTraversal().routes)
    , routeStops(raptorData.getRouteTraversal().routeStops)
    , stopLabelsAndTimes(const_cast<StopLabelsAndTimes&>(stopLabelsAndTimes))
    , minTransferDuration(minimumTransferDuration) {
  }

  std::unordered_set<types::raptorIdx> RouteScanner::scan(const types::raptorInt round, const std::unordered_set<types::raptorIdx>& markedStops) {
    const std::unordered_set<types::raptorIdx> routesToScan = getRoutesToScan(markedStops);
    getConsoleLogger(LoggerName::RAPTOR)->info(std::format("Scanning routes for round {} ({} routes)", round, routesToScan.size()));

    std::unordered_set<types::raptorIdx> markedStopsNext;

    for (const auto currentRouteIdx : routesToScan)
    {
      scanRoute(currentRouteIdx, round, markedStops, markedStopsNext);
    }

    return markedStopsNext;
  }

  std::unordered_set<types::raptorIdx> RouteScanner::getRoutesToScan(const std::unordered_set<types::raptorIdx>& markedStops) const {
    std::unordered_set<types::raptorIdx> routesToScan;

    for (const auto stopIdx : markedStops)
    {
      const Stop& currentStop = stops[stopIdx];
      auto stopRouteIdx = currentStop.stopRouteIndex;
      const auto stopRouteEndIdx = stopRouteIdx + currentStop.numberOfRoutes;

      while (stopRouteIdx < stopRouteEndIdx)
      {
        routesToScan.insert(stopRoutes[stopRouteIdx]);
        ++stopRouteIdx;
      }
    }

    return routesToScan;
  }

  void RouteScanner::scanRoute(const types::raptorIdx currentRouteIdx, const types::raptorInt round, const std::unordered_set<types::raptorIdx>& markedStops, std::unordered_set<types::raptorIdx>& markedStopsNext) {
    const auto lastRound = round - 1;

    const Route& currentRoute = routes[currentRouteIdx];
    getConsoleLogger(LoggerName::RAPTOR)->info(std::format("Scanning route {}", currentRoute.id));

    const auto firstRouteStopIdx = currentRoute.firstRouteStopIndex;
    const auto firstStopTimeIdx = currentRoute.firstStopTimeIndex;
    const auto numberOfStops = currentRoute.numberOfStops;

    std::shared_ptr<ActiveTrip> activeTrip = nullptr;

    for (auto stopOffset = 0u; stopOffset < numberOfStops; stopOffset++)
    {
      const auto stopIdx = routeStops[firstRouteStopIdx + stopOffset].stopIndex;
      const Stop& stop = stops[stopIdx];
      const auto bestStopTime = stopLabelsAndTimes.getComparableBestTime(stopIdx);

      if (activeTrip == nullptr)
      {
        if (!canEnterAtStop(stop, bestStopTime, markedStops, stopIdx, stopOffset, numberOfStops))
        {
          continue;
        }
      }
      else
      {
        const StopTime& stopTimeObj = stopTimes[firstStopTimeIdx + activeTrip->tripOffset * numberOfStops + stopOffset];
        if (!checkIfTripIsPossibleAndUpdateMarks(stopTimeObj, activeTrip, stop, bestStopTime, stopIdx, round, lastRound, markedStopsNext, currentRouteIdx))
        {
          continue;
        }
      }

      activeTrip = findPossibleTrip(stopIdx, stop, stopOffset, currentRoute, lastRound);
    }
  }

  bool RouteScanner::checkIfTripIsPossibleAndUpdateMarks(const StopTime& stopTime, const std::shared_ptr<ActiveTrip>& activeTrip, const Stop& stop, const types::raptorInt bestStopTime, types::raptorIdx stopIdx, const types::raptorInt thisRound, const types::raptorInt lastRound, std::unordered_set<types::raptorIdx>& markedStopsNext, types::raptorIdx currentRouteIdx) const {

    if (const bool isImproved = stopTime.arrival < bestStopTime)
    {
      getConsoleLogger(LoggerName::RAPTOR)->info(std::format("Stop {} was improved", stop.id));
      stopLabelsAndTimes.setBestTime(stopIdx, stopTime.arrival);

      auto label = std::make_unique<StopLabelsAndTimes::Label>(activeTrip->entryTime,
                                                               stopTime.arrival,
                                                               StopLabelsAndTimes::LabelType::ROUTE,
                                                               currentRouteIdx,
                                                               activeTrip->tripOffset,
                                                               stopIdx,
                                                               activeTrip->previousLabel);

      stopLabelsAndTimes.setLabel(thisRound, stopIdx, std::move(label));
      markedStopsNext.insert(stopIdx);

      return false;
    }

    getConsoleLogger(LoggerName::RAPTOR)->info(std::format("Stop {} was not improved", stop.id));
    const auto previous = stopLabelsAndTimes.getLabel(lastRound, stopIdx);

    if (const bool isImprovedInSameRound = previous == nullptr || previous->targetTime >= stopTime.arrival)
    {
      getConsoleLogger(LoggerName::RAPTOR)->info(std::format("Stop {} has been improved in same round, trip not possible within this round", stop.id));
      return false;
    }
    getConsoleLogger(LoggerName::RAPTOR)->info(std::format("Stop {} has not been improved in same round, checking for trips", stop.id));
    return true;
  }

  std::shared_ptr<ActiveTrip> RouteScanner::findPossibleTrip(const types::raptorIdx stopIdx, const Stop& stop, const types::raptorInt stopOffset, const Route& route, const types::raptorInt lastRound) const {
    const auto firstStopTimeIdx = route.firstStopTimeIndex;
    const auto numberOfStops = route.numberOfStops;
    const auto numberOfTrips = route.numberOfTrips;

    types::raptorInt tripOffset = 0;
    auto previousLabel = stopLabelsAndTimes.getLabel(lastRound, stopIdx);

    auto referenceTime = previousLabel->targetTime;
    if (previousLabel->type == StopLabelsAndTimes::LabelType::ROUTE)
    {
      referenceTime += std::max(stop.sameStopTransferTime, minTransferDuration);
    }

    while (tripOffset < numberOfTrips)
    {
      const StopTime& currentStopTime = stopTimes[firstStopTimeIdx + tripOffset * numberOfStops + stopOffset];
      if (currentStopTime.departure >= referenceTime)
      {
        getConsoleLogger(LoggerName::RAPTOR)->info(std::format("Found active trip ({}) on route {}", tripOffset, route.id));
        types::raptorInt entryTime = currentStopTime.departure;
        return std::make_shared<ActiveTrip>(tripOffset, entryTime, previousLabel);
      }
      if (tripOffset < numberOfTrips - 1)
      {
        tripOffset += 1;
      }
      else
      {
        getConsoleLogger(LoggerName::RAPTOR)->info(std::format("No active trip found on route {}", route.id));
        return nullptr;
      }
    }

    return nullptr;
  }
} // raptor