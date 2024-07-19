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
    : timeTable(std::move(relationManager)) {
  }

  std::shared_ptr<IConnection> RaptorStrategy::execute(const std::string& sourceStop, const std::string& targetStop, unsigned int const departureTime) {

    this->sourceStop = sourceStop;
    this->targetStop = targetStop;
    this->startTime = departureTime;
    auto& startStop = timeTable.getData().stops.at(this->sourceStop);
    startStop.earliestArrivalTime = startTime;
    startStop.visited = true;


    this->initialize();
    for (size_t k = 1; k < MAX_ROUNDS + 1; ++k)
    {
      this->updateRoutesToScan();
      this->scanRoutes(k);
      this->scanTransfers(k);
      if (timeTable.getVisitedStopIds().empty())
      {
        break;
      }
    }
    return nullptr;
  }

  void RaptorStrategy::initialize() {

    for (auto& stop : timeTable.getData().stops | std::views::values)
    {
      stop.arrivalTimesKTrips.resize(MAX_ROUNDS + 1);
      std::ranges::fill_n(stop.arrivalTimesKTrips.begin(), MAX_ROUNDS + 1, std::numeric_limits<size_t>::infinity());
    }
    auto& startStop = timeTable.getData().stops.at(sourceStop);
    startStop.arrivalTimesKTrips[0] = startTime;
    startStop.earliestArrivalTime = startTime;

    try
    {
      for (auto& transfer : timeTable.getData().transferFrom.at(startStop.stopId))
      {
        auto& toStop = timeTable.getData().stops.at(transfer.toStopId);
        toStop.arrivalTimesKTrips[0] = startTime;
        toStop.earliestArrivalTime = startTime;
        toStop.transferFromStopId = sourceStop;
        toStop.visited = true;
      }
    } catch (const std::out_of_range& e)
    {
      std::ignore = e;
      getLogger(Target::CONSOLE, LoggerName::RAPTOR)->error(std::format("No transfers found for stop {}", startStop.stopId));
    }
  }

  void RaptorStrategy::updateRoutesToScan() {
    routesToScan.clear();

    for (auto& stop : timeTable.getVisitedStopIds())
    {
      auto& stopItem = timeTable.getData().stops.at(stop);
      for (auto& route : stopItem.routesServedByStop)
      {
        if (auto routeIdIter = routesToScan.find(route);
            routeIdIter != routesToScan.end())
        {
          // TODO compare stop indices instead of stopIds - check if there is an earlier stop
          // if (auto& firstStop = routeIdIter->second; // stopId
          //     // routeObject.isEarlierStop(stop, firstStop))
          //
          // {
          //   firstStop = stop;
          // }
        }
        else
        {
          const auto& route1 = timeTable.getRoutePartitioner().getSubRoutes(route);
          routesToScan.emplace(route, stop);
        }
      }
      stopItem.visited = false; // reset visited flag
    }
  }

  void RaptorStrategy::scanRoutes(size_t const currentRoundK) {
    for (const auto& [routeId, firstStopId] : routesToScan)
    {
      std::optional<std::string> currentTripId = std::nullopt;

      const auto& subRoutesOfRoute = timeTable.getRoutePartitioner().getSubRoutes(routeId);

      for (const auto& subRoute : subRoutesOfRoute)
      {
        auto& subRouteStops = subRoute.getStopsSequence();
        auto& subRouteTrips = subRoute.getTrips();

        // Iterate through stops in the current sub-route starting from firstStopId
        for (size_t stopIndex = subRoute.stopIndex(firstStopId); stopIndex < subRouteStops.size(); ++stopIndex)
        {
          auto& stop = subRouteStops[stopIndex];
          auto& currentStop = timeTable.getData().stops.at(stop.stopId);

          if (currentTripId.has_value())
          {
            // Update stop times if we have a current trip
            const auto& currentTrip = timeTable.getData().trips.at(currentTripId.value());

            if (stopIndex < currentTrip.departureTimesAtStops.size())
            {
              // Prune non-optimal paths
              const auto& targetStop = timeTable.getData().stops.at(stop.stopId);
              const auto& stopTimesCurrentStop = timeTable.getData().stopTimes.at(currentStop.stopId);

              // TODO is not correct, always taking the earliest arrival time of the first stop
              auto minStopTimeCurrent = std::ranges::min_element(stopTimesCurrentStop, [](const auto& aLeft, const auto& aRight) {
                return aLeft.arrivalTime < aRight.arrivalTime;
              });
              auto minStopTimeTarget = std::ranges::min_element(timeTable.getData().stopTimes.at(targetStop.stopId), [](const auto& aLeft, const auto& aRight) {
                return aLeft.arrivalTime < aRight.arrivalTime;
              });
              // TODO is not correct, always taking the earliest arrival time of the first stop
              auto earliestArrivalTime = std::min((*minStopTimeCurrent).arrivalTime, (*minStopTimeTarget).arrivalTime);
              if (const auto currentArrivalTime = currentTrip.departureTimesAtStops[stopIndex];
                  currentArrivalTime < earliestArrivalTime)
              {
                currentStop.arrivalTimesKTrips[currentRoundK] = currentArrivalTime;
                currentStop.earliestArrivalTime = currentArrivalTime;
                currentStop.visited = true;
                currentStop.earliestTripId = currentTripId.value();
                currentStop.earliestTripIdKTrips.insert_or_assign(currentRoundK, currentTripId.value());
              }
            }
          }

          auto currentTime = currentStop.arrivalTimesKTrips[currentRoundK - 1];
          if (currentTime == std::numeric_limits<size_t>::infinity())
          {
            continue;
          }

          // Find the first trip that departs after currentTime
          auto tripIter = std::ranges::find_if(subRouteTrips, [stopIndex, currentTime](const auto& trip) {
            return stopIndex < trip.departureTimesAtStops.size() && trip.departureTimesAtStops[stopIndex] >= currentTime;
          });

          if (tripIter != subRouteTrips.end())
          {
            if (!currentTripId.has_value() || (stopIndex < tripIter->departureTimesAtStops.size() && stopIndex < timeTable.getData().trips.at(currentTripId.value()).departureTimesAtStops.size() && tripIter->departureTimesAtStops[stopIndex] < timeTable.getData().trips.at(currentTripId.value()).departureTimesAtStops[stopIndex]))
            {

              currentTripId = tripIter->tripId;

              // You can also include additional logic here, such as setting boarding stop IDs
              // if (tripIter->boardingStopId.has_value() || (currentStop.transferFromStopId == sourceStop && subRoute.isEarlierStop(currentStop.stopId, tripIter->boardingStopId.value()))) {
              //     timeTable.getData().trips.at(currentTripId.value()).boardingStopId = currentStop.stopId;
              // }
            }
          }
        }
      }
    }
  }



  // auto subRoutesThatServeStop = subRoutesOfRoute | std::views::filter([&](const auto& subRoute) {
  //                                   return std::ranges::any_of(subRoute.getStopsSequence(), [&](const auto& stop) {
  //                                     return stop.stopId == firstStopId;
  //                                   });
  //                                 });
  //
  // for (const auto& route : subRoutesThatServeStop)
  // {
  //   for (const auto& tripInRoute : route.getTrips())
  //   {
  //   }
  // }

  void RaptorStrategy::scanTransfers(const size_t currentRoundK) {
    for (const auto& fromStopId : timeTable.getVisitedStopIds())
    {
      auto& fromStop = timeTable.getData().stops.at(fromStopId);

      for (const auto& toStopObject : timeTable.getData().transferFrom.at(fromStopId))
      {
        auto& toStop = timeTable.getData().stops.at(toStopObject.toStopId);

        if (currentRoundK < MAX_ROUNDS) // || toStop.isArtificial()
        {
          size_t currentTime = fromStop.arrivalTimesKTrips[currentRoundK];

          if (currentTime + toStopObject.minTransferTime >= currentTime)
          {
            currentTime += toStopObject.minTransferTime;
          }

          toStop.arrivalTimesKTrips[currentRoundK] = std::min(toStop.arrivalTimesKTrips[currentRoundK], currentTime);

          if (toStop.arrivalTimesKTrips[currentRoundK] < toStop.earliestArrivalTime)
          {
            toStop.earliestArrivalTime = toStop.arrivalTimesKTrips[currentRoundK];

            toStop.visited = true;
            toStop.transferFromStopId = fromStopId;
          }
        }
      }
    }
  }
} // strategy