//
// Created by MichaelBrunner on 13/06/2024.
//

#include "RelationManager.h"

#include <algorithm>
#include <ranges>


namespace gtfs {
  RelationManager::RelationManager(const GtfsData& data)
    : data(data) {
  }

  void RelationManager::createRelations() {
    collectStopTimesForTrips();
    collectStopTimesForStops();
    collectTripsForRoutes();
    collectRoutesForStops();
  }

  const std::vector<StopTime>& RelationManager::getStopTimesForTrip(const std::string& tripId) const {
    return stopTimeTrips.at(tripId);
  }

  const std::vector<StopTime>& RelationManager::getStopTimesForStop(const std::string& stopId) const {
    return stopTimeStops.at(stopId);
  }

  const std::vector<Trip>& RelationManager::getTripsForRoute(const std::string& routeId) const {
    return tripsRoutes.at(routeId);
  }

  const std::vector<std::string>& RelationManager::getStopsForRoute(const std::string& routeId) const {
    return routesStops.at(routeId);
  }

  void RelationManager::collectStopTimesForTrips() {
    for (const auto& stopTime : data.stopTimes)
    {
      stopTimeTrips[stopTime.tripId].push_back(stopTime);
    }
  }

  void RelationManager::collectStopTimesForStops() {
    for (const auto& stopTime : data.stopTimes)
    {
      stopTimeStops[stopTime.stopId].push_back(stopTime);
    }
  }

  void RelationManager::collectTripsForRoutes() {
    for (const auto& trip : data.trips)
    {
      tripsRoutes[trip.routeId].push_back(trip);
    }
  }

  void RelationManager::collectRoutesForStops() {
    std::ranges::for_each(data.stopTimes, [&](const auto& stopTime) {
      auto tripIter = std::ranges::find_if(data.trips, [&stopTime](const auto& trip) {
        return trip.tripId == stopTime.tripId;
      });

      if (tripIter != data.trips.end())
      {
        routesStops[tripIter->routeId].push_back(stopTime.stopId);
      }
    });
  }
} // gtfs