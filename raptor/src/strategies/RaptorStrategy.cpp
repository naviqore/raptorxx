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
  void removeDominatedLabels(std::vector<utils::Label>& labels, const utils::Label& newLabel) {
    std::erase_if(labels, [&](const utils::Label& label) {
      return label.isDominatedBy(newLabel);
    });
  }

  bool isParetoOptimal(const std::set<utils::Label>& labels, const utils::Label& newLabel) {
    return std::ranges::all_of(labels, [&](const utils::Label& label) {
      return !newLabel.isDominatedBy(label);
    });
  }

  void updateLabels(std::set<utils::Label>& labels, const utils::Label& newLabel) {
    // removeDominatedLabels(labels, newLabel);
    if (isParetoOptimal(labels, newLabel))
    {
      labels.insert(newLabel);
    }
  }

  std::unordered_map<std::string, std::set<utils::Label>> RaptorStrategy::initializeStopLabels(unsigned int const sourceDepartureTime, std::vector<std::string> const& departureStopIds) const {
    std::unordered_map<std::string, std::set<utils::Label>> stopLabels;
    for (const auto& stopId : departureStopIds)
    {
      if (auto stopTimesIter = relationManager.getData().stopTimes.find(stopId); stopTimesIter != relationManager.getData().stopTimes.end())
      {
        for (const auto& stopTime : stopTimesIter->second)
        {
          const auto trips = relationManager.getData().trips.at(stopTime.tripId);
          const auto tripItem = std::ranges::find_if(trips, [&](const schedule::gtfs::Trip& aTrip) { return aTrip.tripId == stopTime.tripId; });
          std::string currentRouteId = tripItem != trips.end() ? tripItem->routeId : "";
          stopLabels[stopId].emplace(utils::Label{sourceDepartureTime, 0, std::nullopt, currentRouteId, stopTime.tripId});
        }
      }
      else
      {
        stopLabels[stopId].emplace(utils::Label{sourceDepartureTime, 0, std::nullopt, "", ""});
      }
    }
    return stopLabels;
  }

  /// @brief
  /// @param relationManager
  RaptorStrategy::RaptorStrategy(schedule::gtfs::RelationManager&& relationManager)
    : relationManager(std::move(relationManager))
    , sourceDepartureTime(0)
    , stopsUpdated(0)
    , stopsReached(0)
    , routesServingUpdatedStops(0) {
  }

  std::vector<std::tuple<std::string, unsigned int, std::string, std::string>> RaptorStrategy::reconstructConnections(const std::unordered_map<std::string, std::set<utils::Label>>& stopLabels, const std::vector<std::string>& arrivalStopIds) {
    std::vector<std::tuple<std::string, unsigned int, std::string, std::string>> path;

    auto allLabels = stopLabels | std::views::keys;
    std::ranges::for_each(allLabels, [&](const auto& label) {
      getLogger(Target::CONSOLE, LoggerName::RAPTOR)->info(std::format("Stop: {}", relationManager.getStopNameFromStopId(label)));
    });

    // Assuming there is only one arrival stop for simplicity
    std::string currentStopId = arrivalStopIds.front();

    while (!currentStopId.empty())
    {
      if (stopLabels.contains(currentStopId) == false)
      {
        break;
      }
      const auto& labels = stopLabels.at(currentStopId);
      auto minLabelIt = std::ranges::min_element(labels, [](const utils::Label& lhs, const utils::Label& rhs) {
        return lhs.arrivalTime < rhs.arrivalTime;
      });

      if (minLabelIt != labels.end())
      {
        path.emplace_back(currentStopId,
                          minLabelIt->arrivalTime,
                          minLabelIt->routeId ? minLabelIt->routeId.value() : "",
                          minLabelIt->tripId ? minLabelIt->tripId.value() : "");
        currentStopId = minLabelIt->previousStopId ? minLabelIt->previousStopId.value() : "";
      }
      else
      {
        currentStopId = ""; // Break condition if no previous stop ID is found
      }
    }

    std::ranges::reverse(path); // Reverse to get the path from source to destination

    return path;
  }

  size_t RaptorStrategy::getTotalNumberOfStops() const {
    return relationManager.getData().stops.size();
  }

  std::unordered_map<std::string, int> RaptorStrategy::createStopIndexMap() const {
    return relationManager.getData().stops
           | std::views::keys
           | std::views::enumerate
           | std::views::transform([](const auto& pair) {
               return std::pair<std::string, int>(std::get<1>(pair), static_cast<int>(std::get<0>(pair)));
             })
           | std::ranges::to<std::unordered_map<std::string, int>>();
  }

  std::vector<schedule::gtfs::StopTime> RaptorStrategy::stopTimesForStopIdAndTimeGreaterThan(std::string const& stopId, unsigned int const departureTime) const {
    return relationManager.getData().stopTimes.at(stopId) | std::views::filter([&](const schedule::gtfs::StopTime& stopTime) {
             return stopTime.departureTime.toSeconds() >= departureTime;
           })
           | std::ranges::to<std::vector>();
  }

  std::optional<int> findEarliestTrip(const std::vector<schedule::gtfs::Trip>& trips, const int stopIndex, const unsigned int currentArrivalTime) {



    // for (int i = 0; i < trips.size(); ++i)
    // {
    //   const auto& tripStopTimes = trips[i].stopTimes;
    //   std::vector<schedule::gtfs::StopTime> tripsWithStop;
    //   std::ranges::transform(tripStopTimes, std::back_inserter(tripsWithStop), [&](const auto& stopTime) {
    //     return stopTime;
    //   });
    //   if (tripsWithStop[stopIndex].departureTime.toSeconds() >= currentArrivalTime)
    //   {
    //     return i;
    //   }
    // }
    return std::nullopt;
  }

  std::shared_ptr<IConnection> RaptorStrategy::execute(const utils::ConnectionRequest& request) {
    // Initialize variables and data structures
    sources = request.departureStopId;
    targets = request.arrivalStopId;
    const unsigned int sourceDepartureTime = request.earliestDepartureTime;

    const auto& [agencies,
                 calendars,
                 calendarDates,
                 routes,
                 stops,
                 stopTimes,
                 transferFrom,
                 transferTo,
                 trips]
      = relationManager.getData();

    std::vector<std::string> allStops;
    std::vector<std::string> allRoutes;
    std::vector<schedule::gtfs::StopTime> allStopTimes;

    std::vector<int> routesIndices;
    std::vector<int> stopsIndices;
    std::vector<int> stopTimesIndices;
    std::vector<size_t> routesStopsSizes;
    std::vector<size_t> stopTimesSizes;

    for (const auto& [routeId, route] : routes)
    {
      routesIndices.push_back(static_cast<int>(allRoutes.size()));
      allRoutes.push_back(routeId);
      routesStopsSizes.push_back(route.stopsInOrder.size());

      for (const auto& trip : route.trips)
      {
        stopTimesSizes.push_back(trip.stopTimes.size());
        for (const auto& stopTime : trip.stopTimes)
        {
          stopTimesIndices.push_back(static_cast<int>(allStopTimes.size()));
          allStopTimes.push_back(stopTime);
        }
      }
      for (const auto& stop : route.stopsInOrder)
      {
        stopsIndices.push_back(static_cast<int>(allStops.size()));
        allStops.push_back(stop.stopId);
      }
    }

    std::map<std::string, int> stopMap;
    for (const auto& [index, stop] : std::views::enumerate(allStops))
    {
      stopMap[stop] = static_cast<int>(index);
    }

    earliestArrival.resize(allStops.size(), utils::LabelEarliestArrival{});
    // utils::LabelEarliestArrival{.arrivalTime = utils::INFINITY_VALUE, .parentDepartureTime = utils::INFINITY_VALUE, .stopId = "", .routeId = "", .useRoute = false}
    std::vector<std::vector<int>> previousStop(MAX_ROUNDS + 1, std::vector<int>(allStops.size(), -1));
    std::vector<std::vector<int>> previousTrip(MAX_ROUNDS + 1, std::vector<int>(allStops.size(), -1));

    if (const auto firstStopIndexInRouteIter = std::ranges::find(allStops, sources.front());
        firstStopIndexInRouteIter != allStops.end())
    {
      const auto firstStopIndexInRoute = std::distance(allStops.begin(), firstStopIndexInRouteIter);
      earliestArrival[firstStopIndexInRoute].arrivalTime = sourceDepartureTime;
      earliestArrival[firstStopIndexInRoute].stopId = sources.front();
    }

    auto lAbelThatIsNotInfinitive = earliestArrival | std::views::filter([](const utils::LabelEarliestArrival& label) {
                                      return label.arrivalTime != utils::INFINITY_VALUE;
                                    });
    if (lAbelThatIsNotInfinitive.empty())
    {
      return nullptr;
    }

    const auto stopName = relationManager.getStopNameFromStopId(lAbelThatIsNotInfinitive.front().stopId);
    getLogger(Target::CONSOLE, LoggerName::RAPTOR)->info(std::format("Stop: {}", stopName));


    auto findStopIndices = [&](const int routeIndex) {
      size_t stopsStartIndex = 0;

      for (int i = 0; i < routeIndex; ++i)
      {
        stopsStartIndex += routes.at(allRoutes[i]).stopsInOrder.size();
      }

      size_t stopsEndIndex = stopsStartIndex + routes.at(allRoutes[routeIndex]).stopsInOrder.size();

      return std::make_pair(stopsStartIndex, stopsEndIndex);
    };


    auto findStopTimesIndices = [&](const int routeIndex, const int tripIndex) {
      size_t stopTimesStartIndex = 0;

      for (int i = 0; i < routeIndex; ++i)
      {
        for (const auto& trip : routes.at(allRoutes[i]).trips)
        {
          stopTimesStartIndex += trip.stopTimes.size();
        }
      }

      for (int i = 0; i < tripIndex; ++i)
      {
        stopTimesStartIndex += routes.at(allRoutes[routeIndex]).trips[i].stopTimes.size();
      }

      size_t stopTimesEndIndex = stopTimesStartIndex + routes.at(allRoutes[routeIndex]).trips[tripIndex].stopTimes.size();

      return std::make_pair(stopTimesStartIndex, stopTimesEndIndex);
    };


    int k = 1;
    bool updated = true; // change to false in future
    while (k < MAX_ROUNDS && updated)
    {
      std::vector<std::pair<int, int>> routeQueue; // queue // (routeIndex, stopIndex)
      // for each marked stop p
      for (int stopIndex = 0; stopIndex < allStops.size(); ++stopIndex)
      {
        if (earliestArrival[stopIndex].arrivalTime != utils::INFINITY_VALUE)
        {
          // for each route r serving p
          for (int routeIndex{0}; routeIndex < routesIndices.size(); ++routeIndex)
          {
            const auto& route = routes.at(allRoutes[routeIndex]);
            const auto& stopsOnRoute = route.stopsInOrder;
            const auto it = std::ranges::find_if(stopsOnRoute, [&](const auto& stop) {
              return stop.stopId == allStops[stopIndex];
            });
            if (it != stopsOnRoute.end())
            {
              getLogger(Target::CONSOLE, LoggerName::RAPTOR)->info(std::format("Stop: {}", it->stopName));
              if (std::ranges::find(routeQueue, std::pair<int, int>(routeIndex, stopIndex)) == routeQueue.end()) // Check if the route is already in the queue ?
              {
                routeQueue.emplace_back(routeIndex, stopIndex);
              }
            }
          }
          // unmark p
          earliestArrival[stopIndex].arrivalTime = utils::INFINITY_VALUE;
        }
      }

      // traverse each route in queue
      for (const auto& [routeIndex, stopIndex] : routeQueue)
      {
        const auto& route = routes.at(allRoutes[routeIndex]);
        const auto& stopsOnRoute = route.stopsInOrder;
        const auto& tripsOnRoute = route.trips;
        const auto numberOfTripsForRoute = tripsOnRoute.size();

        // STOPS
        auto [startOfCurrentRouteStops, endOfCurrentRouteStops] = findStopIndices(routeIndex);
       // const auto startOfCurrentRouteStops = std::accumulate(routesStopsSizes.begin(), routesStopsSizes.begin() + routeIndex, size_t{0});
       // const auto endOfCurrentRouteStops = std::accumulate(routesStopsSizes.begin(), routesStopsSizes.begin() + 1 + routeIndex, size_t{0}); //startOfCurrentRouteStops + (routesStopsSizes[routeIndex]);
        const auto numberOfStopsInCurrentTrip = endOfCurrentRouteStops - startOfCurrentRouteStops;
        const auto differenceCurrentStopIndexToArrayStart = endOfCurrentRouteStops - stopIndex;

        std::optional<int> currentTrip = std::nullopt;

        for (auto stopTimeIndex = startOfCurrentRouteStops; stopTimeIndex < endOfCurrentRouteStops; ++stopTimeIndex) // startOfCurrentRouteStops
        {
          getLogger(Target::CONSOLE, LoggerName::RAPTOR)->info(std::format("Stop: {}", relationManager.getStopNameFromStopId(allStops[stopTimeIndex])));
        }

        // foreach stop p of r beginning with p
        for (auto stopTimeIndex = startOfCurrentRouteStops; stopTimeIndex < endOfCurrentRouteStops; ++stopTimeIndex)
        {
          if (false == currentTrip.has_value())
          {
            // Walk through all trips on the route
            for (int tripIndex = 0; tripIndex < tripsOnRoute.size(); ++tripIndex)
            {
              auto [stopTimesStartIndex, stopTimesEndIndex] = findStopTimesIndices(routeIndex, tripIndex);
              // Compute starting index for StopTimes for the current trip
              // STOP TIMES
              auto tripsRoute = tripsOnRoute[tripIndex];

              getLogger(Target::CONSOLE, LoggerName::RAPTOR)->info(std::format("difference stop times {}", stopTimesEndIndex - stopTimesStartIndex));
              getLogger(Target::CONSOLE, LoggerName::RAPTOR)->info(std::format("difference stop index {}", endOfCurrentRouteStops - startOfCurrentRouteStops));

              for (auto i = stopTimesStartIndex; i < stopTimesEndIndex; ++i)
              {
                getLogger(Target::CONSOLE, LoggerName::RAPTOR)
                  ->info(std::format("StopTimeItem: {} departure Time {}",
                                     relationManager.getStopNameFromStopId(allStopTimes[i].stopId),
                                     allStopTimes[i].departureTime.toString()));
                if (const auto& stopTime = allStopTimes[i];
                    stopTime.departureTime.toSeconds() >= sourceDepartureTime)
                {
                  currentTrip = tripIndex;
                  break;
                }
              }
              // Log all stops in the range
              for (auto i = stopIndex + differenceCurrentStopIndexToArrayStart; i < endOfCurrentRouteStops - 1; ++i) // startOfCurrentRouteStops
              {
                getLogger(Target::CONSOLE, LoggerName::RAPTOR)->info(std::format("Stop: {}", relationManager.getStopNameFromStopId(allStops[i])));
              }


              // auto tt = tripsOnRoute[tripIndex].getStopTimesSorted()[stopIndex];
              // if (tripsOnRoute[tripIndex].getStopTimesSorted()[stopIndex].departureTime.toSeconds() >= sourceDepartureTime) {
              //     currentTrip = tripIndex;
              //     break;
              // }
            }
          }

          if (currentTrip)
          {
            // const auto& trip = tripsOnRoute[*currentTrip];
            // const auto& stopTime = trip.stopTimes[stopTimeIndex];
            //
            // if (earliestArrival[stopIndexOnRoute].arrivalTime > stopTime.arrivalTime.toSeconds())
            // {
            //   earliestArrival[stopIndexOnRoute].arrivalTime = stopTime.arrivalTime.toSeconds();
            //   earliestArrival[stopIndexOnRoute].parentDepartureTime = stopTime.departureTime.toSeconds();
            //   earliestArrival[stopIndexOnRoute].stopId = stopId;
            //   earliestArrival[stopIndexOnRoute].routeId = route.routeId;
            //   earliestArrival[stopIndexOnRoute].useRoute = true;
            //   updated = true;
            //
            //   previousStop[k][stopIndexOnRoute] = stopTimeIndex;
            //   previousTrip[k][stopIndexOnRoute] = *currentTrip;
          }
        }
      }
      ++k;
    }

    return nullptr;
  }
} // namespace raptor::strategy



//       for (const auto& [routeIndex, stopIndex] : routeQueue)
//       {
//         const auto& route = routes.at(allRoutes[routeIndex]);
//         const auto& stopsOnRoute = route.stopsInOrder;
//         auto offset = static_cast<int>(stopsOnRoute.size());
//         const auto& tripsOnRoute = route.trips;
//
//         // StopTimes grouped by trip and sorted by departure time per Route
//         std::vector<std::vector<schedule::gtfs::StopTime>> tripsWithStopTimes; // earliest to latest
//         std::ranges::transform(tripsOnRoute, std::back_inserter(tripsWithStopTimes), [](const auto& trip) {
//           return trip.stopTimes
//                  | std::views::transform([&](const auto& stopTime) { return stopTime; })
//                  | std::ranges::to<std::vector>();
//         });
//         std::ranges::sort(tripsWithStopTimes, [](const std::vector<schedule::gtfs::StopTime>& lhs, const std::vector<schedule::gtfs::StopTime>& rhs) {
//           return lhs.front().departureTime.toSeconds() < rhs.front().departureTime.toSeconds();
//         });
//
//         const auto numberOfTripsInRoute = tripsOnRoute.size();
//         std::optional<int> currentTrip = std::nullopt;
//
//         for (int stopTimeIndex = static_cast<int>(stopsOnRoute.size()) - 1; stopTimeIndex >= 0; --stopTimeIndex)
//         {
//           const auto& stop = stopsOnRoute[stopTimeIndex];
//           const auto& stopId = stop.stopId;
//           const auto& stopIndexOnRoute = stopMap.at(stopId);
//           const auto& stopTimeIndexOnRoute = stopTimesIndices[stopIndexOnRoute];
//           const auto& stopTimeValue = allStopTimes[stopTimeIndexOnRoute];
//
//           if (!currentTrip)
//           {
//             auto stopTimeIndexOfCurrentTrip = std::accumulate(routesStopsSizes.begin(), routesStopsSizes.begin() + static_cast<size_t>(routeIndex), static_cast<size_t>(0));
//             auto stopTimeIndexOnRouteOfCurrentTrip = stopTimesIndices[stopTimeIndexOfCurrentTrip];
//             auto stopTimeItem = allStopTimes[stopTimeIndexOnRouteOfCurrentTrip];
//             if (stopTimeItem.departureTime.toSeconds() >= sourceDepartureTime)
//             {
//               currentTrip = static_cast<int>(stopTimeIndexOfCurrentTrip);
//             }
//
//             /*auto availableStopTimes = stopTimes.at(stopTimeItem.stopId);
//             // get all stopTimes that are serving the stop
//             auto stopTimesServingStop = stopTimes.at(stopId);
//             std::ranges::sort(stopTimesServingStop, [](const schedule::gtfs::StopTime& lhs, const schedule::gtfs::StopTime& rhs) {
//               return lhs.departureTime.toSeconds() < rhs.departureTime.toSeconds();
//             });
//
//             const auto& tripId = stopTimesServingStop.front().tripId;
//             const auto& trip = trips.at(tripId);*/
//           }
//
//           // if current trip
//           // if (earliestArrival[stopIndexOnRoute].arrivalTime > stopTimes.at(allStopTimes[stopTimeIndexOnRoute]).arrivalTime)
//           // {
//           //   earliestArrival[stopIndexOnRoute].arrivalTime = stopTimes.at(allStopTimes[stopTimeIndexOnRoute]).arrivalTime;
//           //   updated = true;
//           // }
//           //
//           // // check if the label of round k-1 improves the active trip
//           // if (earliestArrival[stopIndexOnRoute].arrivalTime <= previousStop[k - 1][stopIndexOnRoute])
//           // {
//           //   previousTrip[k][stopIndexOnRoute] = stopTimeIndex;
//           // }
//         }
//       }
//
//
//       ++k;


/*
*    struct RouteStructure
    {
      std::string routeId;
      std::vector<std::string> routeStops;
      struct Trip
      {
        std::vector<schedule::gtfs::StopTime> stopTimes;
      };

      std::vector<Trip> trips;
    };

    std::vector<RouteStructure> routeStructures;
    for (const auto& route : routes)
    {
      RouteStructure routeStructure;
      routeStructure.routeId = route.first;
      std::ranges::transform(route.second.stopsInOrder, std::back_inserter(routeStructure.routeStops), [](const auto& stop) {
        return stop.stopId;
      });
      for (const auto& trip : route.second.trips)
      {
        RouteStructure::Trip tripStructure;
        tripStructure.stopTimes = {trip.stopTimes.begin(), trip.stopTimes.end()};
        routeStructure.trips.push_back(tripStructure);
      }
      std::ranges::sort(routeStructure.trips, [](const RouteStructure::Trip& lhs, const RouteStructure::Trip& rhs) {
        return lhs.stopTimes.front().departureTime.toSeconds() < rhs.stopTimes.front().departureTime.toSeconds();
      });
      routeStructures.push_back(routeStructure);
    }
 */

/*
 * std::vector<std::string> flatRoutes{};
    std::vector<std::string> routeStopItems{};
    std::vector<std::string> routeStopTimes{};
    for (const auto& routeItem : routes | std::views::values)
    {
      flatRoutes.push_back(routeItem.routeId);
      bool stopItemsAdded = false;
      for (const auto& tripItem : routeItem.trips)
      {
        std::ranges::transform(tripItem.stopTimes, std::back_inserter(routeStopTimes), [](const auto& stopTime) {
          return stopTime.stopId;
        });

        if (stopItemsAdded == false)
        {
          std::ranges::transform(tripItem.stopTimes, std::back_inserter(routeStopItems), [](const auto& stopTime) {
            return stopTime.stopId;
          });
          stopItemsAdded = true;
        }
      }
    }

    std::unordered_map<std::string, int> stopMap;

    for (const auto& [index, routeStop] : std::views::enumerate(routeStopItems))
    {
      stopMap[routeStop] = static_cast<int>(index);
    }

    // Initialize arrival times with infinity
    earliestArrival.resize(routeStopItems.size(), utils::LabelEarliestArrival{.arrivalTime = utils::INFINITY_VALUE});
    std::vector<std::vector<int>> previousStop(MAX_ROUNDS + 1, std::vector<int>(routeStopItems.size(), -1));
    std::vector<std::vector<int>> previousTrip(MAX_ROUNDS + 1, std::vector<int>(routeStopItems.size(), -1));

    auto sourceStopIndex = stopMap.at(sources.front());
    auto targetStopIndex = stopMap.at(targets.front());

    earliestArrival[sourceStopIndex].arrivalTime = sourceDepartureTime;
    earliestArrival[sourceStopIndex].stopId = sources.front();

    int k = 1;
    bool updated = true;

    while (k < MAX_ROUNDS && updated)
    {
      updated = false;
      std::vector<std::pair<int, int>> routeQueue; // (routeIndex, stopIndex)

      // Accumulate routes serving marked stops from previous round
      for (int stopIndex = 0; stopIndex < request.departureStopId.size(); ++stopIndex)
      {
        if (earliestArrival[stopMap[request.departureStopId[stopIndex]]].arrivalTime != utils::INFINITY_VALUE)
        {
          for (const auto& routeItem : routes | std::views::values)
          {
            const auto it = std::ranges::find_if(routeItem.stops, [&](const auto& stopItem) {
              return stopItem.stopId == routeStopItems[stopIndex];
            });
            if (it != routeItem.stops.end())
            {
              routeQueue.emplace_back(static_cast<int>(std::distance(routes.begin(), routes.find(routeItem.routeId))), stopIndex);
            }
          }
          earliestArrival[stopIndex].arrivalTime = utils::INFINITY_VALUE;
        }
      }

      // Traverse each route in queue
      // for (const auto& routeIndex : routeQueue | std::views::keys) {
      //   int currentTrip = -1;
      //   const auto& route = routes.at(flatRoutes[routeIndex]);
      //   for (size_t stopIndex = routeStopItems.size(); stopIndex-- > 0;) {
      //     const int stopTimeIndex = stopIndex;
      //     int stopId = stopMap.at(routeStopItems[stopTimeIndex]);
      //     // Check if current trip improves the arrival time
      //     if (currentTrip == -1 && stopTimes.at(routeStopTimes[stopTimeIndex]).arrivalTime < std::min(earliestArrival[stopId].arrivalTime, earliestArrival[targetStopIndex].arrivalTime)) {
      //       earliestArrival[stopId].arrivalTime = stopTimes.at(routeStopTimes[stopTimeIndex]).arrivalTime;
      //       earliestArrival[targetStopIndex].arrivalTime = stopTimes.at(routeStopTimes[stopTimeIndex]).arrivalTime;
      //       updated = true;
      //     }
      //     // Check if the label of round k-1 improves the active trip
      //     if (earliestArrival[stopId].arrivalTime <= previousStop[k - 1][stopId]) {
      //       currentTrip = stopTimeIndex;
      //     }
      //   }
      // }

      //
      // // Consider foot-paths
      // for (int stopIndex = 0; stopIndex < numStops; ++stopIndex)
      // {
      //   if (earliestArrival[stopIndex].arrivalTime != utils::INFINITY_VALUE)
      //   {
      //     for (const auto& transfer : transferFrom.at(stopIndex))
      //     {
      //       int targetStopIndex = stopMap.at(transfer.toStopId);
      //       int travelTime = transfer.duration;
      //       if (earliestArrival[targetStopIndex].arrivalTime > earliestArrival[stopIndex].arrivalTime + travelTime)
      //       {
      //         earliestArrival[targetStopIndex].arrivalTime = earliestArrival[stopIndex].arrivalTime + travelTime;
      //         updated = true;
      //       }
      //     }
      //     // Mark the stop
      //     earliestArrival[stopIndex].arrivalTime = utils::INFINITY_VALUE;
      //   }
      // }
      //
      // if (updated)
      // {
      //   std::vector<utils::LabelEarliestArrival> newRound(numStops, utils::LabelEarliestArrival{.arrivalTime = utils::INFINITY_VALUE});
      //   rounds.push_back(newRound);
      // }

      ++k;
    }
*/


/*
 *std::shared_ptr<IConnection> RaptorStrategy::execute(const utils::ConnectionRequest& request) {
    // Initialize variables and data structures
    sources = request.departureStopId;
    targets = request.arrivalStopId;
    const unsigned int sourceDepartureTime = request.earliestDepartureTime;

    const auto numStops = getTotalNumberOfStops();

    earliestArrival.resize(numStops, utils::LabelEarliestArrival{});
    std::vector<std::vector<int>> previousStop(MAX_ROUNDS + 1, std::vector<int>(numStops, -1));
    std::vector<std::vector<int>> previousTrip(MAX_ROUNDS + 1, std::vector<int>(numStops, -1));

    stopMap = createStopIndexMap();

    int sourceStopIndex = stopMap.at(sources.front());
    int targetStopIndex = stopMap.at(targets.front());

    earliestArrival[sourceStopIndex].arrivalTime = sourceDepartureTime;

    const auto& [agencies,
                 calendars,
                 calendarDates,
                 routes,
                 stops,
                 stopTimes,
                 transferFrom,
                 transferTo,
                 trips]
      = relationManager.getData();

    flatStopIds.reserve(stops.size());
    flatStopIds = stops | std::views::keys | std::ranges::to<std::vector>();

    tripMap = trips | std::views::keys | std::views::enumerate | std::views::transform([](const auto& pair) {
                return std::pair<std::string, int>(std::get<1>(pair), static_cast<int>(std::get<0>(pair)));
              })
              | std::ranges::to<std::unordered_map<std::string, int>>();


    arrivalTimes.reserve(stops.size());

    // First round
    const auto stopTimesForStopId = stopTimesForStopIdAndTimeGreaterThan(sources.front(), sourceDepartureTime);
    const auto tripsForStopTime = stopTimesForStopId | std::views::transform([&](const schedule::gtfs::StopTime& stopTime) {
                                    return trips.at(stopTime.tripId);
                                  })
                                  | std::ranges::to<std::vector>();

    earliestArrival[sourceStopIndex].arrivalTime = sourceDepartureTime;
    earliestArrival[sourceStopIndex].stopId = sources.front();

    const auto initialRoute = tripsForStopTime.front().front().routeId;
    const auto initialRouteToScan = routes.at(initialRoute);
    const auto stopsOfInitialRoute = initialRouteToScan.stops;

    const auto allRoutes = routes | std::views::keys | std::ranges::to<std::vector>();

    std::vector<std::string> routeBuiltWithSortedStopTimes{};

    std::vector<std::string> allStopTimeItems;
    std::vector<std::string> allStopItems;

    for (const auto& tripItems : trips | std::views::values)
    {
      bool routeStopsAdded = false;
      for (const auto& tripItem : tripItems)
      {
        for (const auto& route = routes.at(tripItem.routeId);
             const auto& stopItem : route.stops)
        {
          // allStopItems.push_back(stopItem.stopId);
          auto values = tripItem.stopTimes | std::views::transform([](const auto& stopTime) {
                          return stopTime.stopId;
                        });
          allStopTimeItems.insert(allStopTimeItems.end(), values.begin(), values.end());
          if (routeStopsAdded == false)
          {
            allStopItems.insert(allStopItems.end(), values.begin(), values.end());
            routeStopsAdded = true;
          }
        }
      }
    }

    std::vector<std::vector<utils::LabelEarliestArrival>> rounds;
    rounds.emplace_back(allStopItems.size());

    auto& currentRound = rounds.back();
    currentRound[sourceStopIndex].arrivalTime = sourceDepartureTime;
    currentRound[sourceStopIndex].stopId = sources.front();


    int k = 1;
    bool updated = true;



    while (k < MAX_ROUNDS)
    {
      const auto& stopTimesForStop = relationManager.getStopTimesFromStopIdStartingFromSpecificTime(sources.front(), sourceDepartureTime);
      for (const auto& stopTime : stopTimesForStop)
      {
        const auto& tripItems = relationManager.getData().trips.at(stopTime.tripId);
        for (const auto& tripItem : tripItems)
        {
          for (const auto& stop : tripItem.stopTimes)
          {
            const auto& stopIndex = stopMap.at(stop.stopId);

            if (currentRound[stopIndex].arrivalTime > stopTime.departureTime.toSeconds())
            {
              currentRound[stopIndex].arrivalTime = stopTime.arrivalTime.toSeconds();
              currentRound[stopIndex].stopId = stop.stopId;
              currentRound[stopIndex].useRoute = true;
            }
          }
        }
      }
      ++k;
    }

    // Return null pointer for now, as the connection construction is not fully implemented
    return nullptr;
  }
  */


/*
 *    auto stopLabels = initializeStopLabels(sourceDepartureTime, departureStopIds);
    std::unordered_map<std::string, unsigned int> stopEarliestArrivalTime;

    std::vector<utils::LabelEarliestArrival> rounds;

    for (const auto& stopId : departureStopIds)
    {
      stopEarliestArrivalTime[stopId] = sourceDepartureTime;
    }

    for (const auto& stopId : departureStopIds)
    {
     // rou
    }



    std::set<std::string> markedStops(departureStopIds.begin(), departureStopIds.end());

    for (unsigned int round = 1; round <= MAX_ROUNDS; ++round) // for each round k <-- 1, 2, ..., K
    {
      std::unordered_map<std::string, utils::Label> newStopLabels;
      std::queue<std::pair<std::string, std::string>> Q;
      std::set<std::pair<std::string, std::string>> seen; // To avoid duplicates

      // Accumulate routes serving marked stops without duplicates
      for (const auto& stopId : markedStops)
      {
        auto stopTimesIter = relationManager.getData().stopTimes.find(stopId);
        auto dropBasedOnTime = stopTimesIter->second | std::views::drop_while([&](const schedule::gtfs::StopTime& stopTime) {
                                 return stopTime.departureTime.toSeconds() < stopEarliestArrivalTime[stopId] && stopTime.arrivalTime.toSeconds() < stopEarliestArrivalTime[arrivalStopIds.front()];
                               });

        if (auto stopTimesIter = relationManager.getData().stopTimes.find(stopId);
            stopTimesIter != relationManager.getData().stopTimes.end())
        {
          auto stopTimesGreaterThanMinTime = stopTimesIter->second | std::views::filter([&](const schedule::gtfs::StopTime& stopTime) {
                                               return stopTime.departureTime.toSeconds() >= stopEarliestArrivalTime[stopId];
                                             })
                                             | std::ranges::to<std::vector>();

          std::ranges::sort(stopTimesGreaterThanMinTime, [](const schedule::gtfs::StopTime& lhs, const schedule::gtfs::StopTime& rhs) {
            return lhs.departureTime.toSeconds() < rhs.departureTime.toSeconds();
          });
          auto firstStopTime = stopTimesGreaterThanMinTime.front();
          getLogger(Target::CONSOLE, LoggerName::RAPTOR)->info(std::format("Stop: {}, Time: {}, Route: {}", relationManager.getStopNameFromStopId(firstStopTime.stopId), firstStopTime.departureTime.toSeconds(), firstStopTime.tripId));
          // for (const auto& stopTime : stopTimesGreaterThanMinTime)
          // {
          //   LoggingPool::getInstance(Target::CONSOLE)->info(std::format("Stop: {}, Time: {}, Route: {}", relationManager.getStopNameFromStopId(stopTime.stopId), stopTime.departureTime.toSeconds(), stopTime.tripId));
          //  //  const auto& trip = relationManager.getData().trips.at(stopTime.tripId);
          //   // Q.emplace(trip.front().routeId, stopId);
          // }
        }
      }
    }


    return nullptr;

*/


// // Initialization
// auto stopLabels = initializeStopLabels(sourceDepartureTime, departureStopIds);
// std::unordered_map<std::string, unsigned int> earliestArrivalTime;
// for (const auto& stopId : departureStopIds)
// {
//   earliestArrivalTime[stopId] = sourceDepartureTime;
// }
// std::set<std::string> markedStops(departureStopIds.begin(), departureStopIds.end());
//
// for (unsigned int round = 1; round <= MAX_ROUNDS; ++round)
// {
//   std::unordered_map<std::string, utils::Label> newStopLabels;
//   std::queue<std::pair<std::string, std::string>> Q;
//
//   // Accumulate routes serving marked stops
//   for (const auto& stopId : markedStops)
//   {
//     if (auto stopTimesIter = relationManager.getData().stopTimes.find(stopId); stopTimesIter != relationManager.getData().stopTimes.end())
//     {
//       for (const auto& stopTime : stopTimesIter->second)
//       {
//         const auto& trip = relationManager.getData().trips.at(stopTime.tripId); // size should be one !! TODO check
//         Q.emplace(trip.front().routeId, stopId);
//       }
//     }
//   }
//   markedStops.clear();
//
//   // Traverse each route
//   while (!Q.empty())
//   {
//     auto [routeId, startStop] = Q.front();
//     Q.pop();
//     const auto& route = relationManager.getData().routes.at(routeId);
//     bool foundActiveTrip = false;
//     utils::Label activeLabel;
//
//     for (const auto& stop : route.stops | std::views::drop_while([&](const schedule::gtfs::Stop& stop) { return stop.stopId != startStop; }))
//     {
//       const std::string& stopId = stop.stopId;
//
//       if (foundActiveTrip && activeLabel.time < std::min(earliestArrivalTime[stopId], earliestArrivalTime[arrivalStopIds.front()]))
//       {
//         earliestArrivalTime[stopId] = activeLabel.time;
//         newStopLabels[stopId] = activeLabel;
//         markedStops.insert(stopId);
//       }
//
//       if (auto it = stopLabels.find(stopId); it != stopLabels.end())
//       {
//         const auto& labels = it->second;
//         auto bestLabel = std::ranges::min_element(labels, [](const utils::Label& lhs, const utils::Label& rhs) { return lhs.time < rhs.time; });
//         const auto& stopTime = relationManager.getStopTimeFromStopId(stop.stopId);
//         if (bestLabel != labels.end() && bestLabel->time <= stopTime.departureTime.toSeconds())
//         {
//           activeLabel = *bestLabel;
//           activeLabel.time = stopTime.arrivalTime.toSeconds();
//           activeLabel.transfers++;
//           foundActiveTrip = true;
//         }
//       }
//     }
//   }
//
//   // Look at foot-paths
//   for (const auto& stopId : markedStops)
//   {
//     if (auto transfersIter = relationManager.getData().transferFrom.find(stopId);
//         transfersIter != relationManager.getData().transferFrom.end())
//     {
//       for (const auto& transfer : transfersIter->second)
//       {
//         const auto& fromStopId = transfer.fromStopId;
//         const auto& toStopId = transfer.toStopId;
//
//         unsigned int transferTime = transfer.minTransferTime;
//         unsigned int newArrivalTime = earliestArrivalTime[fromStopId] + transferTime;
//
//         if (newArrivalTime < earliestArrivalTime[toStopId])
//         {
//           earliestArrivalTime[toStopId] = newArrivalTime;
//           newStopLabels[toStopId] = {newArrivalTime, newStopLabels[fromStopId].transfers + 1, fromStopId, "", ""};
//           markedStops.insert(toStopId);
//         }
//       }
//     }
//   }
//
//   // Update stop labels with new labels
//   for (const auto& [stopId, newLabel] : newStopLabels)
//   {
//     updateLabels(stopLabels[stopId], newLabel);
//   }
//
//   // Stopping criterion
//   if (markedStops.empty())
//   {
//     break;
//   }
// }
//
// // Reconstruct the connection from target back to source
// const auto path = reconstructConnections(stopLabels, arrivalStopIds);
//
// // Logging trips and stops in the reconstructed path
// LoggingPool::getInstance(Target::CONSOLE)->info(" ");
// LoggingPool::getInstance(Target::CONSOLE)->info("Reconstructed path:");
// std::string currentTrip = "";
// for (const auto& [stopId, time, routeId, tripId] : path)
// {
//   if (tripId != currentTrip)
//   {
//     if (!currentTrip.empty())
//     {
//       LoggingPool::getInstance(Target::CONSOLE)->info(" ");
//     }
//     currentTrip = tripId;
//     LoggingPool::getInstance(Target::CONSOLE)->info(std::format("Trip: {}", tripId));
//   }
//   LoggingPool::getInstance(Target::CONSOLE)->info(std::format("Stop: {}, Time: {}, Route: {}", stopId, time, routeId));
//
//   std::ranges::for_each(relationManager.getData().routes.at(routeId).stops, [&](const schedule::gtfs::Stop& stop) {
//     LoggingPool::getInstance(Target::CONSOLE)->info(std::format("Stop: {}, Name: {}", stop.stopId, stop.stopName));
//   });
// }
//
// return nullptr;



// const auto& departureStopIds = request.departureStopId;
// const auto& arrivalStopIds = request.arrivalStopId;
// const unsigned int sourceDepartureTime = request.earliestDepartureTime;
//
// // Initialize stop labels
// auto stopLabels = initializeStopLabels(sourceDepartureTime, departureStopIds);
//
// // Perform RAPTOR algorithm rounds
// for (unsigned int round = 0; round < MAX_ROUNDS; ++round)
// {
//   std::unordered_map<std::string, utils::Label> newStopLabels;
//
//   for (const auto& [stopId, labels] : stopLabels)
//   {
//     for (const auto& label : labels)
//     {
//       // get stop times for current stop
//       auto stopTimesIter = relationManager.getData().stopTimes.find(stopId);
//       if (stopTimesIter == relationManager.getData().stopTimes.end()) continue; // if no stop times found for stop, continue
//
//       for (const auto& stopTime : stopTimesIter->second)
//       {
//         if (stopTime.departureTime.toSeconds() < label.time) continue;
//
//         auto tripIter = relationManager.getData().trips.find(stopTime.tripId);
//         if (tripIter == relationManager.getData().trips.end()) continue; // if no trip found for stop time, continue
//
//         for (const auto& trip : tripIter->second)
//         {
//           try
//           {
//             const auto& route = relationManager.getData().routes.at(trip.routeId);
//             bool foundArrival = false;
//             // check if arrival stop is in route stops - if yes, calculate arrival time and update label
//             for (const auto& arrivalStopId : arrivalStopIds)
//             {
//               if (auto arrivalStopIter = std::ranges::find_if(route.stops, [&](const schedule::gtfs::Stop& stop) {
//                     return stop.stopId == arrivalStopId; // check if arrival stop is in route stops
//                   });
//                   arrivalStopIter != route.stops.end())
//               {
//                 foundArrival = true;
//                 const unsigned int travelTime = stopTime.departureTime.toSeconds() - label.time;
//                 const unsigned int arrivalTime = label.time + travelTime;
//                 const int tripCount = label.transfers + 1;
//
//                 if (utils::Label newLabel = {arrivalTime, tripCount, stopId, trip.routeId, stopTime.tripId};
//                     !newStopLabels.contains(arrivalStopId) || isParetoOptimal({newStopLabels[arrivalStopId]}, newLabel))
//                 {
//                   newStopLabels[arrivalStopId] = newLabel;
//                 }
//               }
//             }
//
//             if (!foundArrival)
//             {
//               try
//               {
//                 for (const auto& transfer : relationManager.getData().transferFrom.at(stopId))
//                 {
//                   if (transfer.fromStopId == stopId)
//                   {
//                     const unsigned int travelTime = stopTime.departureTime.toSeconds() - label.time;
//                     const unsigned int arrivalTime = label.time + travelTime + transfer.minTransferTime;
//                     const int tripCount = label.transfers + 1;
//
//                     if (utils::Label newLabel = {arrivalTime, tripCount, stopId, trip.routeId, stopTime.tripId};
//                         !newStopLabels.contains(transfer.toStopId) || isParetoOptimal({newStopLabels[transfer.toStopId]}, newLabel))
//                     {
//                       newStopLabels[transfer.toStopId] = newLabel;
//                     }
//                   }
//                 }
//               } catch (std::out_of_range& e)
//               {
//                 LoggingPool::getInstance(Target::CONSOLE)->error(std::format("Error: {} {}", e.what(), "No transfers found for stop ID: " + stopId));
//               }
//             }
//           } catch (std::out_of_range& e)
//           {
//             LoggingPool::getInstance(Target::CONSOLE)->error(std::format("Error: {} {}", e.what(), "No route found for trip ID: " + trip.routeId));
//           }
//         }
//       }
//     }
//   }
//
//   for (const auto& [stopId, newLabel] : newStopLabels)
//   {
//     updateLabels(stopLabels[stopId], newLabel);
//   }
// }
//
// // Reconstruct the connection from target back to source
// const auto path = reconstructConnections(stopLabels, arrivalStopIds);
//
// // Logging trips and stops in the reconstructed path
// LoggingPool::getInstance(Target::CONSOLE)->info(" ");
// LoggingPool::getInstance(Target::CONSOLE)->info("Reconstructed path:");
// std::string currentTrip = "";
// for (const auto& [stopId, time, routeId, tripId] : path)
// {
//   if (tripId != currentTrip)
//   {
//     if (!currentTrip.empty())
//     {
//       LoggingPool::getInstance(Target::CONSOLE)->info(" ");
//     }
//     currentTrip = tripId;
//     LoggingPool::getInstance(Target::CONSOLE)->info(std::format("Trip: {}", tripId));
//   }
//   LoggingPool::getInstance(Target::CONSOLE)->info(std::format("Stop: {}, Time: {}, Route: {}", stopId, time, routeId));
//
//   std::ranges::for_each(relationManager.getData().routes.at(routeId).stops, [&](const schedule::gtfs::Stop& stop) {
//     LoggingPool::getInstance(Target::CONSOLE)->info(std::format("Stop: {}, Name: {}", stop.stopId, stop.stopName));
//   });
// }
//
// return nullptr;