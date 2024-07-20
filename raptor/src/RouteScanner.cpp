//
// Created by MichaelBrunner on 20/07/2024.
//

#include "RouteScanner.h"

namespace raptor {

  RouteScanner::RouteScanner(const StopLabelsAndTimes& stopLabelsAndTimes, const RaptorData& raptorData, int minimumTransferDuration)
    : stops(raptorData.getStopContext().stops)
    , stopRoutes(raptorData.getStopContext().stopRoutes)
    , stopTimes(raptorData.getRouteTraversal().stopTimes)
    , routes(raptorData.getRouteTraversal().routes)
    , routeStops(raptorData.getRouteTraversal().routeStops)
    , stopLabelsAndTimes(const_cast<StopLabelsAndTimes&>(stopLabelsAndTimes))
    , minTransferDuration(minimumTransferDuration) {
    // Constructor implementation
  }

  std::unordered_set<int> RouteScanner::scan(int round, const std::unordered_set<int>& markedStops) {
    std::unordered_set<int> routesToScan = getRoutesToScan(markedStops);
    std::cout << "Scanning routes for round " << round << " (" << routesToScan.size() << " routes)" << std::endl;

    std::unordered_set<int> markedStopsNext;

    for (int currentRouteIdx : routesToScan)
    {
      scanRoute(currentRouteIdx, round, markedStops, markedStopsNext);
    }

    return markedStopsNext;
  }

  std::unordered_set<int> RouteScanner::getRoutesToScan(const std::unordered_set<int>& markedStops) {
    std::unordered_set<int> routesToScan;

    for (int stopIdx : markedStops)
    {
      const Stop& currentStop = stops[stopIdx];
      int stopRouteIdx = currentStop.stopRouteIndex;
      int stopRouteEndIdx = stopRouteIdx + currentStop.numberOfRoutes;

      while (stopRouteIdx < stopRouteEndIdx)
      {
        routesToScan.insert(stopRoutes[stopRouteIdx]);
        ++stopRouteIdx;
      }
    }

    return routesToScan;
  }

  void RouteScanner::scanRoute(int currentRouteIdx, int round, const std::unordered_set<int>& markedStops, std::unordered_set<int>& markedStopsNext) {
    const int lastRound = round - 1;

    const Route& currentRoute = routes[currentRouteIdx];
    std::cout << "Scanning route " << currentRoute.id << std::endl;

    const int firstRouteStopIdx = currentRoute.firstRouteStopIndex;
    const int firstStopTimeIdx = currentRoute.firstStopTimeIndex;
    const int numberOfStops = currentRoute.numberOfStops;

    std::shared_ptr<ActiveTrip> activeTrip = nullptr;


    for (int stopOffset = 0; stopOffset < numberOfStops; stopOffset++)
    {
      int stopIdx = routeStops[firstRouteStopIdx + stopOffset].stopIndex;
      const Stop& stop = stops[stopIdx];
      int bestStopTime = stopLabelsAndTimes.getComparableBestTime(stopIdx);

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

  bool RouteScanner::canEnterAtStop(const Stop& stop, int stopTime, const std::unordered_set<int>& markedStops, int stopIdx, int stopOffset, int numberOfStops) {
    int unreachableValue = std::numeric_limits<int>::max();

    if (stopTime == unreachableValue)
    {
      std::cout << "Stop " << stop.id << " cannot be reached, continue" << std::endl;
      return false;
    }

    if (!markedStops.contains(stopIdx))
    {
      std::cout << "Stop " << stop.id << " was not improved in previous round, continue" << std::endl;
      return false;
    }

    if (stopOffset + 1 == numberOfStops)
    {
      std::cout << "Stop " << stop.id << " is last stop in route, continue" << std::endl;
      return false;
    }
    std::cout << "Got first entry point at stop " << stop.id << " at " << stopTime << std::endl;

    return true;
  }

  bool RouteScanner::checkIfTripIsPossibleAndUpdateMarks(const StopTime& stopTime, std::shared_ptr<ActiveTrip> activeTrip, const Stop& stop, int bestStopTime, int stopIdx, int thisRound, int lastRound, std::unordered_set<int>& markedStopsNext, int currentRouteIdx) {
    bool isImproved = stopTime.arrival < bestStopTime;

    if (isImproved)
    {
      std::cout << "Stop " << stop.id << " was improved" << std::endl;
      stopLabelsAndTimes.setBestTime(stopIdx, stopTime.arrival);

      auto label = std::make_shared<StopLabelsAndTimes::Label>(activeTrip->entryTime,
                                                               stopTime.arrival,
                                                               StopLabelsAndTimes::LabelType::ROUTE,
                                                               currentRouteIdx,
                                                               activeTrip->tripOffset,
                                                               stopIdx,
                                                               activeTrip->previousLabel);

      stopLabelsAndTimes.setLabel(thisRound, stopIdx, label);
      markedStopsNext.insert(stopIdx);

      return false;
    }
    else
    {
      std::cout << "Stop " << stop.id << " was not improved" << std::endl;
      auto previous = stopLabelsAndTimes.getLabel(lastRound, stopIdx);

      bool isImprovedInSameRound = previous == nullptr || previous->targetTime >= stopTime.arrival;

      if (isImprovedInSameRound)
      {
        std::cout << "Stop " << stop.id << " has been improved in same round, trip not possible within this round" << std::endl;
        return false;
      }
      else
      {
        std::cout << "Checking for trips at stop " << stop.id << std::endl;
        return true;
      }
    }
  }

  std::shared_ptr<raptor::RouteScanner::ActiveTrip> RouteScanner::findPossibleTrip(int stopIdx, const Stop& stop, int stopOffset, const Route& route, int lastRound) {
    int firstStopTimeIdx = route.firstStopTimeIndex;
    int numberOfStops = route.numberOfStops;
    int numberOfTrips = route.numberOfTrips;

    int tripOffset = 0;
    int entryTime = 0;
    auto previousLabel = stopLabelsAndTimes.getLabel(lastRound, stopIdx);

    int referenceTime = previousLabel->targetTime;
    if (previousLabel->type == StopLabelsAndTimes::LabelType::ROUTE)
    {
      referenceTime += std::max(stop.sameStopTransferTime, minTransferDuration);
    }

    while (tripOffset < numberOfTrips)
    {
      const StopTime& currentStopTime = stopTimes[firstStopTimeIdx + tripOffset * numberOfStops + stopOffset];
      if (currentStopTime.departure >= referenceTime)
      {
        std::cout << "Found active trip (" << tripOffset << ") on route " << route.id << std::endl;
        entryTime = currentStopTime.departure;
        return std::make_shared<raptor::RouteScanner::ActiveTrip>(tripOffset, entryTime, previousLabel);
      }
      if (tripOffset < numberOfTrips - 1)
      {
        tripOffset += 1;
      }
      else
      {
        std::cout << "No active trip found on route " << route.id << std::endl;
        return nullptr;
      }
    }

    return nullptr;
  }
} // raptor