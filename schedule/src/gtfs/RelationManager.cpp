//
// Created by MichaelBrunner on 13/06/2024.
//

#include "RelationManager.h"

#include <algorithm>
#include <ranges>
#include <utility>


namespace schedule::gtfs {

  RelationManager::RelationManager(GtfsData&& data)
    : data(std::move(data)) {
    createRelations();
  }

  void RelationManager::createRelations() {
    // Reserve capacity if possible, e.g., data.trips.size() as an estimate
    for (const auto& stopTimes : data.stopTimes | std::views::values)
    {
      for (const auto& stopTime : stopTimes)
      {
        if (auto tripIter = data.trips.find(stopTime.tripId);
            tripIter != data.trips.end())
        {
          for (auto& trip : tripIter->second)
          {
            trip.stopTimes.insert(stopTime);
          }
        }
      }
    }

    for (auto& trips : data.trips | std::views::values)
    {
      for (auto& trip : trips)
      {
        auto& route = data.routes.at(trip.routeId); // Use operator[] to access or insert
        route.trips.push_back(trip);
      }
    }

    for (auto& route : data.routes | std::views::values)
    {
      std::vector<std::string> uniqueStopIds;
      bool added = false;
      for (const auto& trip : route.trips)
      {
        if (added) { break; }
        for (const auto& stopTime : trip.stopTimes)
        {
          uniqueStopIds.push_back(stopTime.stopId);
        }
        added = true;
      }
      for (const auto& stopId : uniqueStopIds)
      {
        route.stopsInOrder.push_back(data.stops.at(stopId));
      }
    }
  }

  const GtfsData& RelationManager::getData() const {
    return data;
  }

  const std::string& RelationManager::getStopNameFromStopId(std::string const& aStopId) const {
    return data.stops.at(aStopId).stopName;
  }
  const std::string& RelationManager::getStopIdFromStopName(std::string const& aStopName) const {
    return std::ranges::find_if(data.stops,
                                [&aStopName](const auto& stop) { return stop.second.stopName == aStopName; })
      ->first;
  }
  std::vector<std::string> RelationManager::getStopIdsFromStopName(std::string const& aStopName) const {
    return data.stops | std::views::filter([&aStopName](const auto& stop) { return stop.second.stopName == aStopName; })
           | std::views::keys | std::ranges::to<std::vector<std::string>>();
  }
  const StopTime& RelationManager::getStopTimeFromStopId(std::string const& aStopId) const {
    return data.stopTimes.at(aStopId).front();
  }

  std::vector<Stop> RelationManager::getAllStopsOnTrip(std::string const& aTripId) const {
    // Retrieve all StopTimes for the given tripId, sorted by departure time
    auto stopTimesMatchingTripId
      = data.stopTimes | std::views::values | std::views::join
        | std::views::filter([&](const StopTime& stopTime) { return stopTime.tripId == aTripId; })
        | std::ranges::to<std::vector<StopTime>>();

    std::ranges::sort(stopTimesMatchingTripId,
                      [](const StopTime& a, const StopTime& b) { return a.departureTime < b.departureTime; });

    std::vector<Stop> stopsForTrip;
    for (const auto& stopTime : stopTimesMatchingTripId)
    { stopsForTrip.push_back(data.stops.at(stopTime.stopId)); }

    return stopsForTrip;
  }

  const std::vector<StopTime>& RelationManager::getStopTimesFromStopId(std::string const& aStopId) const {
    return data.stopTimes.at(aStopId);
  }
  std::vector<Route> RelationManager::getRouteFromTripId(std::string const& aTripId) const {
    return data.trips.at(aTripId)
           | std::views::transform([this](const Trip& trip) { return data.routes.at(trip.routeId); })
           | std::ranges::to<std::vector<Route>>();
  }
  std::vector<StopTime> RelationManager::getStopTimesFromStopIdStartingFromSpecificTime(
    std::string const& aStopId, unsigned int secondsGreaterThan) const {
    return data.stopTimes.at(aStopId) | std::views::filter([secondsGreaterThan, this](const StopTime& stopTime) {
             return stopTime.departureTime.toSeconds() > secondsGreaterThan;
           })
           | std::ranges::to<std::vector<StopTime>>();
  }

  bool RelationManager::isServiceActiveOnDay(std::string const& aServiceId, const std::chrono::weekday aDay) const {
    return data.calendars.at(aServiceId).weekdayService.contains(aDay);
  }

  const std::vector<Trip>& RelationManager::getTripsFromStopTimeTripId(std::string const& aTripId) const {
    return data.trips.at(aTripId);
  }

} // gtfs
