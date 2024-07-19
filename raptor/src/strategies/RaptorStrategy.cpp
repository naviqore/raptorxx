//
// Created by MichaelBrunner on 27/06/2024.
//

#include "RaptorStrategy.h"
#include "ConnectionImpl.h"
#include "LegImpl.h"
#include <LoggerFactory.h>
#include <format>
#include <utility>
#include "raptorTypes.h"
#include <algorithm>
#include <numeric>
#include <queue>
#include <set>
#include <stack>
#include <vector>
#include <tuple>
#include <ranges>
#include <optional>
#include <utils/Label.h>

namespace raptor::strategy {

  RaptorStrategy::RaptorStrategy(schedule::gtfs::TimetableManager&& relationManager)
    : gtfsData(std::move(relationManager))
    , stopsUpdated(static_cast<int>(this->gtfsData.getData().stops.size()))
    , stopsReached(static_cast<int>(this->gtfsData.getData().stops.size()))
    , routesServingUpdatedStops(static_cast<int>(this->gtfsData.getData().routes.size())) {
    earliestArrival.assign(gtfsData.getData().stops.size(), std::numeric_limits<int>::max());
    previousRoundArrival.assign(gtfsData.getData().stops.size(), std::numeric_limits<int>::max());

    initialize();
    initializeStopEventsOfTrip();

    raptorUtil.initialize(gtfsData.getData());
  }

  std::shared_ptr<IConnection> RaptorStrategy::execute(const std::string& sourceStop, const std::string& targetStop, int departureTime) {
    source = sourceStop;
    target = targetStop;
    earliestDepartureTime = departureTime;
    sourceLatestDepartureTime = earliestDepartureTime + 60 * 60; // 1 hour later

    bestTimes.assign(gtfsData.getData().stops.size(), std::numeric_limits<int>::infinity());
    bestTimes[raptorUtil.getStopIndexFromId(sourceStop)] = departureTime;

    // collectDepartureTimes();
    // initialize();
    //
    // int k = 0;
    // while (k < MAX_ROUNDS)
    // {
    //   relaxTransfers();
    //   startNewRound();
    //   collectRoutesServingUpdatedStops();
    //   scanRoutes();
    //   ++k;
    // }
    return nullptr;
  }

  void RaptorStrategy::collectDepartureTimes() {
    collectedDepTimes.clear();
    for (const auto& [routeId, route] : gtfsData.getData().routes)
    {
      for (const auto& trip : route.trips)
      {
        for (const auto& stopTime : trip.stopTimes)
        {
          if (stopTime.departureTime >= earliestDepartureTime && stopTime.departureTime <= sourceLatestDepartureTime)
          {
            collectedDepTimes.emplace_back(DepartureLabel{stopTime.departureTime, stopTime.stopId});
          }
        }
      }
    }
    std::sort(collectedDepTimes.begin(), collectedDepTimes.end(), [](const DepartureLabel& a, const DepartureLabel& b) {
      return a.depTime > b.depTime;
    });
  }

  void RaptorStrategy::initialize() {
    clear();
    const int sourceIndex = stopIdIndexMap[source];
    updateArrivalTime(sourceIndex, earliestDepartureTime);
    stopsReached.insert(sourceIndex);
    currentRound()[stopIdIndexMap[source]].parentDepartureTime = earliestDepartureTime;
    currentRound()[stopIdIndexMap[source]].useRoute = false;
    currentRound()[stopIdIndexMap[source]].routeId = "";
    // allStops = std::views::keys(gtfsData.getData().stops) | std::ranges::to<std::vector<std::string>>();
    // for (int i = 0; i < allStops.size(); ++i)
    // {
    //   stopIdIndexMap[allStops[i]] = i;
    // }
    // allRoutes = std::views::keys(gtfsData.getData().routes) | std::ranges::to<std::vector<std::string>>();
    // for (int i = 0; i < allRoutes.size(); ++i)
    // {
    //   routeIdIndexMap[allRoutes[i]] = i;
    // }
  }
  void RaptorStrategy::initializeStopEventsOfTrip() {
    stopEventsOfTrip.clear();
    stopEventsOfTrip.resize(gtfsData.getData().trips.size());

    int routeIndex = 0;
    for (const auto& [routeId, route] : gtfsData.getData().routes)
    {
      for (const auto& trip : route.trips)
      {
        stopEventsOfTrip[routeIndex].resize(trip.stopTimes.size());
        for (int stopTimeIndex = 0; stopTimeIndex < trip.stopTimes.size(); ++stopTimeIndex)
        {
          const auto& stopTime = trip.getStopTimesSorted()[stopTimeIndex];
          stopEventsOfTrip[routeIndex][stopTimeIndex] = {stopTime.departureTime, stopTime.arrivalTime};
        }
      }
      routeIdIndexMap[routeId] = routeIndex;
      ++routeIndex;
    }

    int stopIndex = 0;
    for (const auto& stopId : gtfsData.getData().stops | std::views::keys)
    {
      stopIdIndexMap[stopId] = stopIndex;
      allStops.push_back(stopId);
      ++stopIndex;
    }

    for (const auto& routeId : gtfsData.getData().routes | std::views::keys)
    {
      allRoutes.push_back(routeId);
    }
  }

  void RaptorStrategy::clear() {
    earliestArrival.assign(gtfsData.getData().stops.size(), std::numeric_limits<int>::max());
    rounds.emplace_back(gtfsData.getData().stops.size(), utils::LabelEarliestArrival{});
    stopsUpdated.clear();
    stopsReached.clear();
    routesServingUpdatedStops.clear();
  }

  bool RaptorStrategy::updateArrivalTime(int stopIndex, int arrivalTime) {
    if (earliestArrival[stopIndex] > arrivalTime)
    {
      earliestArrival[stopIndex] = arrivalTime;
      stopsUpdated.insert(stopIndex);
      stopsReached.insert(stopIndex);
      return true;
    }
    return false;
  }

  void RaptorStrategy::relaxTransfers() {
    for (const int stopIndex : stopsUpdated.getElements())
    {
      if (const std::string& stopId = allStops[stopIndex];
          gtfsData.getData().transferFrom.contains(stopId))
      {
        for (const auto& transfer : gtfsData.getData().transferFrom.at(stopId))
        {
          const int toStopIndex = stopIdIndexMap[transfer.toStopId];
          const int newArrivalTime = earliestArrival[stopIndex] + transfer.minTransferTime;
          if (updateArrivalTime(toStopIndex, newArrivalTime))
          {
            currentRound()[toStopIndex].stopId = transfer.toStopId;
            currentRound()[toStopIndex].parentDepartureTime = earliestArrival[stopIndex];
            currentRound()[toStopIndex].useRoute = false;
            currentRound()[toStopIndex].routeId = "";
          }
        }
      }
    }
  }

  void RaptorStrategy::startNewRound() {
    // initialize a new vector of LabelEarliestArrival and add it to the rounds vector
    rounds.emplace_back(gtfsData.getData().stops.size(), utils::LabelEarliestArrival{});
  }

  void RaptorStrategy::collectRoutesServingUpdatedStops() {
    routesServingUpdatedStops.clear();

    for (const int stopIndex : stopsUpdated.getElements())
    {
      const std::string& stopId = allStops[stopIndex];
      for (const auto& [routeId, route] : gtfsData.getData().routes)
      {
        for (const auto& trip : route.trips)
        {
          for (size_t i = 0; i < trip.stopTimes.size(); ++i)
          {
            if (trip.getStopTimesSorted()[i].stopId == stopId)
            {
              routesServingUpdatedStops.insert(routeIdIndexMap[routeId], static_cast<int>(i));
              break;
            }
          }
        }
      }
    }
  }
  void RaptorStrategy::scanRoutes() {
    stopsUpdated.clear();

    // for (const auto& [routeIndex, stopIndex] : routesServingUpdatedStops.getElements())
    // {
    //   const auto& routeId = allRoutes[routeIndex];
    //   const auto& tripsForRoute = gtfsData.getData().routes.at(routeId).trips;
    //
    //   for (const auto& trip : tripsForRoute)
    //   {
    //     auto stopsOnTrip = trip.getStopTimesSorted();
    //   }

    // if (tripsForRoute.empty()) continue;
    //
    // for (const auto& stopIndex : stopsReached.getElements())
    // {
    //   int earliestBoardingTime = std::numeric_limits<int>::max();
    //   int tripIndexForEarliestBoarding = -1;
    //   int stopIndexInTrip = -1;
    //
    //   // Find the earliest trip that can be boarded at this stop
    //   for (int tripIndex = 0; tripIndex < tripsForRoute.size(); ++tripIndex)
    //   {
    //     const auto& trip = tripsForRoute[tripIndex];
    //     for (int stopTimeIndex = 0; stopTimeIndex < trip.stopTimes.size(); ++stopTimeIndex)
    //     {
    //       const auto stopTime = trip.getStopTimesSorted()[stopTimeIndex];
    //       auto testObject = gtfsData.getData().stops.at(stopTime.stopId);
    //       auto id1 = allStops[stopIndex];
    //       auto id2 = stopTime.stopId;
    //       if (stopTime.stopId == allStops[stopIndex] && static_cast<int>(stopTime.departureTime.toSeconds()) >= earliestArrival[stopIndex])
    //       {
    //         if (static_cast<int>(stopTime.departureTime.toSeconds()) < earliestBoardingTime)
    //         {
    //           earliestBoardingTime = static_cast<int>(stopTime.departureTime.toSeconds());
    //           tripIndexForEarliestBoarding = tripIndex;
    //           stopIndexInTrip = stopTimeIndex;
    //           break; // Found earliest boarding time for this trip
    //         }
    //       }
    //     }
    //   }
    //
    //   if (tripIndexForEarliestBoarding == -1) continue; // No trip found that can be boarded
    //
    //   // Update arrival times for all subsequent stops in the trip
    //   const auto& trip = tripsForRoute[tripIndexForEarliestBoarding];
    //   for (int stopTimeIndex = stopIndexInTrip + 1; stopTimeIndex < trip.stopTimes.size(); ++stopTimeIndex)
    //   {
    //     const auto& stopTime = trip.getStopTimesSorted()[stopTimeIndex];
    //     int stopIndexForNextStop = stopIdIndexMap[stopTime.stopId];
    //     if (updateArrivalTime(stopIndexForNextStop, stopTime.arrivalTime))
    //     {
    //       stopsUpdated.insert(stopIndexForNextStop);
    //       currentRound()[stopIndexForNextStop].parentDepartureTime = earliestBoardingTime;
    //       currentRound()[stopIndexForNextStop].useRoute = true;
    //       currentRound()[stopIndexForNextStop].routeId = routeId;
    //     }
    //   }
    // }
    // }
  }

  int RaptorStrategy::getFirstTripOfRoute(const std::string& routeId) const {
    auto routeIndex = getRouteIndex(routeId);
    return routeIndex < static_cast<int>(stopEventsOfTrip.size()) ? static_cast<int>(stopEventsOfTrip[routeIndex][0].arrivalTime) : -1;
  }

  int RaptorStrategy::getLastTripOfRoute(const std::string& routeId) const {
    auto routeIndex = getRouteIndex(routeId);
    return routeIndex < static_cast<int>(stopEventsOfTrip.size()) ? static_cast<int>(stopEventsOfTrip[routeIndex].back().arrivalTime) : -1;
  }
  int RaptorStrategy::getRouteIndex(const std::string& routeId) const {
    auto it = routeIdIndexMap.find(routeId);
    return it != routeIdIndexMap.end() ? it->second : -1;
  }
  std::vector<utils::LabelEarliestArrival>& RaptorStrategy::currentRound() {
    return rounds.back();
  }
} // namespace raptor::strategy



// routesServingUpdatedStops.clear();
// for (const auto& stopsUpdatedElements = stopsUpdated.getElements();
//      const auto stop : stopsUpdatedElements)
// {
//   if (const auto& stopId = allStops[stop];
//       relationManager.getData().transferFrom.contains(stopId))
//   {
//     for (const auto& transfer : relationManager.getData().transferFrom.at(stopId))
//     {
//       const auto& toStopId = transfer.toStopId;
//       const auto& transferTime = transfer.minTransferTime;
//       const auto& fromStopId = transfer.fromStopId;
//       const auto& fromStopIndex = stopIdIndexMap[fromStopId];
//       const auto& toStopIndex = stopIdIndexMap[toStopId];
//
//       if (const auto& arrivalTime = earliestArrival[fromStopIndex] + transferTime;
//           arrivalTime < earliestArrival[toStopIndex])
//       {
//         if (updateArrivalTime(toStopIndex, arrivalTime))
//         {
//           stopsUpdated.insert(toStopIndex);
//           currentRound()[toStopIndex].stopId = toStopId;
//           currentRound()[toStopIndex].parentDepartureTime = earliestArrival[fromStopIndex];
//           currentRound()[toStopIndex].routeId = "";
//           currentRound()[toStopIndex].useRoute = false;
//         }
//       }
//     }
//   }
// }



// Iterate through stops that have been updated in the current round
// for (const auto& stopIndex : stopsUpdated.getElements())
// {
//   const auto arrivalTime = this->previousRound()[stopIndex].arrivalTime;
//   const std::string& stopId = allStops[stopIndex];
//
//   // Find routes that contain the current stop
//   for (const auto& route : allRoutes)
//   {
//     const auto& currentRoute = relationManager.getData().routes.at(route);
//
//     // Check if route contains the current stop via stopId
//     if (auto it = std::ranges::find_if(currentRoute.stopsInOrder, [stopId](const schedule::gtfs::Stop& stop) {
//           return stop.stopId == stopId;
//         });
//         it != currentRoute.stopsInOrder.end())
//     {
//       const auto stopEventsTrip = stopEventsOfTrip[currentRoute.trips.size() - 1];
//       if (stopEventsTrip[stopIndex].departureTime < arrivalTime)
//       {
//         continue;
//       }
//       // Store the route that serves the updated stop
//       routesServingUpdatedStops.insert(routeIdIndexMap[route], stopIndex);
//     }
//   }
// }



// std::vector<schedule::gtfs::StopTime> RaptorStrategy::getStopTimesForTrip(const int routeIndex, const int tripIndex) const {
//   const auto startIdx = tripOffsets[routeIndex] + tripIndex;
//   const auto endIdx = tripIndex + 1 < routeObjects[routeIndex].trips.size() ? tripOffsets[routeIndex] + tripIndex + 1 : flatStopTimes.size();
//   return {flatStopTimes.begin() + startIdx, flatStopTimes.begin() + static_cast<int>(endIdx)};
// }
// /// @brief
// /// @param relationManager
// RaptorStrategy::RaptorStrategy(schedule::gtfs::RelationManager&& relationManager)
//   : gtfsData(std::move(relationManager))
//   , stopsUpdated(static_cast<int>(this->gtfsData.getData().stops.size()))
//   , stopsReached(static_cast<int>(this->gtfsData.getData().stops.size()))
//   , routesServingUpdatedStops(static_cast<int>(this->gtfsData.getData().routes.size())) {
//   initialize();
// }
//
//
// std::shared_ptr<IConnection> RaptorStrategy::execute(const utils::ConnectionRequest& request) {
//   // Initialize variables and data structures
//   source = request.departureStopId.front();
//   target = request.arrivalStopId.front();
//   earliestDepartureTime = static_cast<int>(request.earliestDepartureTime);
//   sourceLatestDepartureTime = earliestDepartureTime + 60 * 60; // 1 hour later
//
//   const auto& [agencies,
//                calendars,
//                calendarDates,
//                routes,
//                stops,
//                stopTimes,
//                transferFrom,
//                transferTo,
//                trips]
//     = relationManager.getData();
//
//   allStops = std::views::keys(stops) | std::ranges::to<std::vector<std::string>>();
//   allRoutes = std::views::keys(routes) | std::ranges::to<std::vector<std::string>>();
//   routeObjects = std::views::values(routes) | std::ranges::to<std::vector<schedule::gtfs::Route>>();
//
//   for (int i = 0; i < allStops.size(); ++i)
//   {
//     stopIdIndexMap[allStops[i]] = i;
//   }
//
//   for (int i = 0; i < allRoutes.size(); ++i)
//   {
//     routeIdIndexMap[allRoutes[i]] = i;
//   }
//
//   flatStopTimes.clear();
//   routeOffsets.clear();
//   tripOffsets.clear();
//
//   int stopTimeIndex = 0;
//
//   for (const auto& route : routes)
//   {
//     routeOffsets.push_back(stopTimeIndex);
//     for (const auto& trip : route.second.trips)
//     {
//       tripOffsets.push_back(stopTimeIndex);
//       for (const auto& stopTime : trip.stopTimes)
//       {
//         flatStopTimes.push_back(stopTime);
//         ++stopTimeIndex;
//       }
//     }
//   }
//
//   this->collectDepartureTimes();
//   this->initialize();
//
//   auto sourceDepartureTime = 0;
//
//   int k = 0;
//   while (k < MAX_ROUNDS)
//   {
//     this->relaxTransfers();
//     this->startNewRound();
//     this->collectRoutesServingUpdatedStops();
//     this->scanRoutes();
//     sourceDepartureTime = collectedDepTimes[k].depTime;
//     ++k;
//   }
//
//
//   return nullptr;
// }
//
// void RaptorStrategy::collectDepartureTimes() {
//
//   for (const auto& route : relationManager.getData().routes | std::views::values)
//   {
//     auto trips = route.trips; // Copy needed since getData() returns a const reference
//     std::ranges::sort(trips, [](const schedule::gtfs::Trip& a, const schedule::gtfs::Trip& b) {
//       return a.getStopTimesSorted().front().arrivalTime < b.getStopTimesSorted().front().arrivalTime;
//     });
//
//     for (const auto& trip : trips)
//     {
//       const auto& stopTimesPerTrip = trip.getStopTimesSorted();
//       for (const auto& [index, stopTime] : std::views::enumerate(stopTimesPerTrip))
//       {
//         if (index + 1 == stopTimesPerTrip.size()) continue; // Skip last stop sequence
//         if (static_cast<int>(stopTime.departureTime.toSeconds()) < earliestDepartureTime
//             || static_cast<int>(stopTime.departureTime.toSeconds()) > sourceLatestDepartureTime) continue;
//
//         if (const auto transferTime = getTransferTime(stopTime.stopId, source);
//             stopTime.stopId == source || transferTime < std::numeric_limits<int>::infinity())
//         {
//           collectedDepTimes.emplace_back(stopTime.departureTime.toSeconds(), stopTime.stopId);
//         }
//       }
//     }
//   }
//   // Sort the collected departure times in reverse order
//   std::ranges::sort(collectedDepTimes, [](const DepartureLabel& a, const DepartureLabel& b) {
//     return a.depTime > b.depTime;
//   });
// }
//
// int RaptorStrategy::getTransferTime(const std::string& fromStopId, const std::string& toStopId) const {
//   const auto& transferFrom = relationManager.getData().transferFrom;
//   if (const auto transferIter = transferFrom.find(fromStopId); transferIter != transferFrom.end())
//   {
//     for (const auto& transfer : transferIter->second)
//     {
//       if (transfer.toStopId == toStopId)
//       {
//         return transfer.minTransferTime;
//       }
//     }
//   }
//   return std::numeric_limits<int>::max();
// }
//
// void RaptorStrategy::initialize() {
//   this->clear();
//   this->updateArrivalTime(stopIdIndexMap[source], earliestDepartureTime);
//   this->currentRound()[stopIdIndexMap[source]].parentDepartureTime = earliestDepartureTime;
//   this->currentRound()[stopIdIndexMap[source]].useRoute = false;
//   this->currentRound()[stopIdIndexMap[source]].routeId = "";
// }
// void RaptorStrategy::clear() {
//   std::ranges::for_each(stopIdIndexMap | std::views::values, [this](const int stopIndex) {
//     earliestArrival.push_back(std::numeric_limits<int>::max());
//   });
//   std::vector<utils::LabelEarliestArrival> round;
//   for (const auto& _ : stopIdIndexMap | std::views::keys)
//   {
//     round.emplace_back(utils::LabelEarliestArrival{});
//   }
//   rounds.push_back(std::move(round));
//   stopsUpdated = utils::IndexedVisitedSet{static_cast<int>(stopIdIndexMap.size())};
//   stopsReached = utils::IndexedVisitedSet{static_cast<int>(stopIdIndexMap.size())};
//   routesServingUpdatedStops = utils::IndexedVisitedHashMap{static_cast<int>(relationManager.getData().routes.size())};
// }
//
// bool RaptorStrategy::updateArrivalTime(int const stopIndex, int const arrivalTime) {
//   if (earliestArrival[stopIndex] > arrivalTime)
//   {
//     currentRound()[stopIndex].arrivalTime = arrivalTime;
//     earliestArrival[stopIndex] = arrivalTime;
//     stopsUpdated.insert(stopIndex);
//     return true;
//   }
//   return false;
// }
//
// std::vector<utils::LabelEarliestArrival>& RaptorStrategy::currentRound() {
//   return rounds.back();
// }
// std::vector<utils::LabelEarliestArrival>& RaptorStrategy::previousRound() {
//   return rounds[rounds.size() - 2];
// }
//
// void RaptorStrategy::relaxTransfers() {
//   for (const auto& stopIndex : stopsUpdated.getElements())
//   {
//     const std::string& stopId = allStops[stopIndex];
//     if (relationManager.getData().transferFrom.contains(stopId))
//     {
//       for (const auto& transfer : relationManager.getData().transferFrom.at(stopId))
//       {
//         const std::string& toStopId = transfer.toStopId;
//         int transferTime = transfer.minTransferTime;
//         int toStopIndex = stopIdIndexMap[toStopId];
//         int newArrivalTime = earliestArrival[stopIndex] + transferTime;
//
//         if (updateArrivalTime(toStopIndex, newArrivalTime))
//         {
//           currentRound()[toStopIndex].stopId = toStopId;
//           currentRound()[toStopIndex].parentDepartureTime = earliestArrival[stopIndex];
//           currentRound()[toStopIndex].routeId = "";
//           currentRound()[toStopIndex].useRoute = false;
//         }
//       }
//     }
//   }
// }
//
// void RaptorStrategy::startNewRound() {
//   std::vector<utils::LabelEarliestArrival> round(stopIdIndexMap.size());
//   rounds.push_back(std::move(round));
// }
//
// std::vector<schedule::gtfs::Route> RaptorStrategy::routesContainingStop(int aStop) const {
//
//   return relationManager.getData().routes | std::views::values
//          | std::views::filter([aStop, this](const schedule::gtfs::Route& route) {
//              return std::ranges::find_if(route.stopsInOrder,
//                                          [this, aStop](const schedule::gtfs::Stop& stop) { return stop.stopId == allStops[aStop]; })
//                     != route.stopsInOrder.end();
//            })
//          | std::ranges::to<std::vector<schedule::gtfs::Route>>();
// }
//
// void RaptorStrategy::collectRoutesServingUpdatedStops() {
//   routesServingUpdatedStops.clear();
//   for (const auto& stopIndex : stopsUpdated.getElements())
//   {
//     const std::string& stopId = allStops[stopIndex];
//     for (const auto& route : relationManager.getData().routes | std::views::values)
//     {
//       auto it = std::ranges::find_if(route.stopsInOrder, [stopId](const schedule::gtfs::Stop& stop) {
//         return stop.stopId == stopId;
//       });
//       if (it != route.stopsInOrder.end())
//       {
//         routesServingUpdatedStops.insert(routeIdIndexMap[route.routeId], stopIndex);
//       }
//     }
//   }
// }
// void RaptorStrategy::scanRoutes() {
//   stopsUpdated.clear();
//
//   for (const auto& [routeIndex, stopIndex] : routesServingUpdatedStops.getElements())
//   {
//     const auto& routeId = allRoutes[routeIndex];
//     const auto& routeObject = relationManager.getData().routes.at(routeId);
//     int firstTrip = getFirstTripOfRoute(routeId);
//     auto trip = getLastTripOfRoute(routeId) - 1;
//
//     int currentStopIndex = stopIndex;
//     int parentIndex = stopIndex;
//     const auto& stopSequence = routeObject.stopsInOrder;
//     std::string stop = allStops[currentStopIndex];
//
//     // Loop over the stops
//     while (currentStopIndex < routeObject.stopsInOrder.size() - 1)
//     {
//       // Find trip to "hop on"
//       while (trip > firstTrip && stopEventsOfTrip[trip - 1][currentStopIndex].departureTime >= previousRound()[stopIdIndexMap[stop]].arrivalTime)
//       {
//         --trip;
//         parentIndex = currentStopIndex;
//       }
//
//       ++currentStopIndex;
//       stop = allStops[currentStopIndex];
//
//       if (updateArrivalTime(stopIdIndexMap[stop], stopEventsOfTrip[trip][currentStopIndex].arrivalTime))
//       {
//         stopsReached.insert(stopIdIndexMap[stop]);
//         currentRound()[stopIdIndexMap[stop]].parentStopId = allStops[parentIndex];
//         currentRound()[stopIdIndexMap[stop]].useRoute = true;
//         currentRound()[stopIdIndexMap[stop]].parentDepartureTime = stopEventsOfTrip[trip][parentIndex].departureTime;
//         currentRound()[stopIdIndexMap[stop]].routeId = routeId;
//       }
//
//       ++currentStopIndex;
//     }
//   }
// }
// int RaptorStrategy::getRouteIndex(const std::string& routeId) const {
//   for (size_t i = 0; i < routeObjects.size(); ++i)
//   {
//     if (routeObjects[i].routeId == routeId)
//     {
//       return static_cast<int>(i);
//     }
//   }
//   throw std::invalid_argument("Route ID not found");
// }s