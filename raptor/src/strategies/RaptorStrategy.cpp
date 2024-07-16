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



  std::shared_ptr<IConnection> RaptorStrategy::execute(const utils::ConnectionRequest& request) {

    const std::vector<std::string>& sourceStops = request.departureStopId;
    const auto& targetStops = request.arrivalStopId;
    const unsigned int sourceDepartureTime = request.earliestDepartureTime;

    const auto numStops = getTotalNumberOfStops();

    std::vector<std::vector<unsigned int>> earliestArrival(MAX_ROUNDS + 1, std::vector<unsigned int>(numStops, utils::INFINITY_VALUE));
    std::vector<std::vector<int>> previousStop(MAX_ROUNDS + 1, std::vector<int>(numStops, -1)); // -1 means no previous visited stop
    std::vector<std::vector<int>> previousTrip(MAX_ROUNDS + 1, std::vector<int>(numStops, -1));

    const auto stopIndexMap = createStopIndexMap();

    int sourceStopIndex = stopIndexMap.at(sourceStops.front());
    int targetStopIndex = stopIndexMap.at(targetStops.front());

    earliestArrival[0][sourceStopIndex] = sourceDepartureTime;

    const auto& gtfsDataRelationManger = relationManager.getData();
    const auto& trips = gtfsDataRelationManger.trips;

    const auto stopTimesForStopId = stopTimesForStopIdAndTimeGreaterThan(sourceStops.front(), sourceDepartureTime);

    // filter trips from stopTime tripId
    const auto tripsForStopTime = stopTimesForStopId | std::views::transform([&](const schedule::gtfs::StopTime& stopTime) {
                                    return trips.at(stopTime.tripId);
                                  })
                                  | std::ranges::to<std::vector>();

    for (int round = 1; round <= MAX_ROUNDS; ++round)
    {
      bool updated = false;
      for (const auto& tripsServingStop : tripsForStopTime)
      {
        for (const auto& trip : tripsServingStop)
        {
          getLogger(Target::CONSOLE, LoggerName::RAPTOR)->info(std::format("Trip: {}", trip.tripId));
        }
      }
    }
    return nullptr;
  }
  
} // namespace raptor::strategy



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