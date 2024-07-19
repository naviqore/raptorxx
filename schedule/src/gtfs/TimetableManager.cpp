//
// Created by MichaelBrunner on 13/06/2024.
//

#include "TimetableManager.h"

#include <algorithm>
#include <ranges>
#include <utility>


namespace schedule::gtfs {

  TimetableManager::TimetableManager(GtfsData&& data)
    : data(std::move(data)) {
    createRelations();
  }

  void TimetableManager::createRelations() {
    // Reserve capacity if possible, e.g., data.trips.size() as an estimate
    for (const auto& stopTimes : data.stopTimes | std::views::values)
    {
      for (const auto& stopTime : stopTimes)
      {
        if (auto tripIter = data.trips.find(stopTime.tripId);
            tripIter != data.trips.end())
        {
          tripIter->second.stopTimes.insert(stopTime);
        }
      }
    }

    for (auto& trip : data.trips | std::views::values)
    {
      auto& route = data.routes.at(trip.routeId);
      route.trips.push_back(trip);

      // for (auto& trip : trips)
      // {
      //   auto& route = data.routes.at(trip.routeId); // Use operator[] to access or insert
      //   route.trips.push_back(trip);
      // }
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
        route.stops.push_back(data.stops.at(stopId));
      }
    }
  }
  void TimetableManager::buildTripsToRoutesRelations() {
    for (auto& [tripId, trip] : data.trips)
    {
      // data.routes.at(trip.routeId).trips.push_back(&trip); // dangerous but let's try with pointers
      data.routes.at(trip.routeId).trips.push_back(trip.tripId);
      trip.routeId = data.routes.at(trip.routeId).routeId;
    }
  }
  void TimetableManager::buildStopTimesToTripsAndRoutesRelations() {
    std::optional<std::string> scannedTripId;
    bool addStops = false;


    for (const auto& stopTime : data.stopTimes | std::views::values | std::views::join)
    {
      // Add stopTime to trip
      auto& trip = data.trips.at(stopTime.tripId);
      trip.addTimesAtStop(stopTime.arrivalTime.toSeconds(), stopTime.departureTime.toSeconds());

      auto& stop = data.stops.at(stopTime.stopId);
      // Add stopTime to route and stop if not already present
      if (std::ranges::find_if(stop.routesServedByStop, [&trip](const std::string& routeId) { return routeId == trip.routeId; })
          == stop.routesServedByStop.end())
      {
        stop.routesServedByStop.push_back(trip.routeId);
      }

      if (scannedTripId != stopTime.tripId)
      {
        scannedTripId = stopTime.tripId;
        // route is scanned for the first time
        if (const auto& route = data.routes.at(trip.routeId);
            route.stops.empty())
        {
          addStops = true;
        }
      }
      if (addStops)
      {
        data.routes.at(trip.routeId).stops.push_back(stop.stopId);
      }
    }
  }

  const GtfsData& TimetableManager::getData() const {
    return data;
  }

  const std::string& TimetableManager::getStopNameFromStopId(std::string const& aStopId) const {
    return data.stops.at(aStopId).stopName;
  }
  const std::string& TimetableManager::getStopIdFromStopName(std::string const& aStopName) const {
    return std::ranges::find_if(data.stops,
                                [&aStopName](const auto& stop) { return stop.second.stopName == aStopName; })
      ->first;
  }
  std::vector<std::string> TimetableManager::getStopIdsFromStopName(std::string const& aStopName) const {
    return data.stops | std::views::filter([&aStopName](const auto& stop) { return stop.second.stopName == aStopName; })
           | std::views::keys | std::ranges::to<std::vector<std::string>>();
  }
  const StopTime& TimetableManager::getStopTimeFromStopId(std::string const& aStopId) const {
    return data.stopTimes.at(aStopId).front();
  }

  std::vector<Stop> TimetableManager::getAllStopsOnTrip(std::string const& aTripId) const {
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

  const std::vector<StopTime>& TimetableManager::getStopTimesFromStopId(std::string const& aStopId) const {
    return data.stopTimes.at(aStopId);
  }
  Route TimetableManager::getRouteFromTripId(std::string const& aTripId) const {
    const auto& trip = data.trips.at(aTripId);
    return data.routes.at(trip.routeId);

    // return data.trips.at(aTripId)
    //        | std::views::transform([this](const Trip& trip) { return data.routes.at(trip.routeId); })
    //        | std::ranges::to<std::vector<Route>>();
  }
  std::vector<StopTime> TimetableManager::getStopTimesFromStopIdStartingFromSpecificTime(
    std::string const& aStopId, unsigned int secondsGreaterThan) const {
    return data.stopTimes.at(aStopId) | std::views::filter([secondsGreaterThan, this](const StopTime& stopTime) {
             return stopTime.departureTime.toSeconds() > secondsGreaterThan;
           })
           | std::ranges::to<std::vector<StopTime>>();
  }

  bool TimetableManager::isServiceActiveOnDay(std::string const& aServiceId, const std::chrono::weekday aDay) const {
    return data.calendars.at(aServiceId).weekdayService.contains(aDay);
  }

  const Trip& TimetableManager::getTripsFromStopTimeTripId(std::string const& aTripId) const {
    return data.trips.at(aTripId);
  }

  std::vector<std::string> TimetableManager::getVisitedStopIds() {
    return std::ranges::views::keys(data.stops)
           | std::views::filter([&](const std::string& stopId) { return data.stops.at(stopId).visited; })
           | std::ranges::to<std::vector<std::string>>();
  }

} // gtfs
