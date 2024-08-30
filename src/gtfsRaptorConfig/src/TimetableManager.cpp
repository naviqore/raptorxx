//
// Created by MichaelBrunner on 13/06/2024.
//

#include "TimetableManager.h"

#include <algorithm>
#include <ranges>
#include <utility>


namespace converter {

  TimetableManager::TimetableManager(schedule::gtfs::GtfsData&& data)
    : data(std::make_unique<schedule::gtfs::GtfsData>(std::move(data)))
  {
    createRelations();
    routePartitioner = std::make_unique<RoutePartitioner>(this->data.get());
  }

  void TimetableManager::createRelations() const
  {
    buildTripsToRoutesRelations();
    buildStopTimesToTripsAndRoutesRelations();
  }

  void TimetableManager::buildTripsToRoutesRelations() const
  {
    std::ranges::for_each(data->trips | std::views::values, [this](const schedule::gtfs::Trip& trip) {
      data->routes.at(trip.routeId).trips.push_back(trip.tripId);
    });
  }

  void TimetableManager::buildStopTimesToTripsAndRoutesRelations() const
  {
    std::string scannedTripId{};
    bool addStops{false};

    for (const auto& stopTime : data->stopTimes | std::views::values | std::views::join) {
      // Add stopTime to trip
      schedule::gtfs::Trip& tripServingStopTime = data->trips.at(stopTime.tripId);
      tripServingStopTime.stopTimes.push_back(stopTime);
      // tripServingStopTime.stopTimes.push_back(&stopTime);
      // TODO check if this is necessary
      // tripServingStopTime.addTimesAtStop(stopTime.arrivalTime.toSeconds(), stopTime.departureTime.toSeconds());

      // Stop that is served by the stopTime
      // Add stopTime to route and stop if not already present
      // if (Stop& stopServedByStopTime = data->stops.at(stopTime.stopId);
      //     std::ranges::find_if(stopServedByStopTime.routesServedByStop, [&tripServingStopTime](const std::string& routeId) { return routeId == tripServingStopTime.routeId; })
      //     == stopServedByStopTime.routesServedByStop.end())
      // {
      //   stopServedByStopTime.routesServedByStop.push_back(tripServingStopTime.routeId);
      // }
    }
  }

  const schedule::gtfs::GtfsData& TimetableManager::getData() const
  {
    return *data;
  }

  schedule::gtfs::GtfsData& TimetableManager::getData()
  {
    return *data;
  }
  RoutePartitioner& TimetableManager::getRoutePartitioner()
  {
    return *routePartitioner;
  }

  const RoutePartitioner& TimetableManager::getRoutePartitioner() const
  {
    return *routePartitioner;
  }

  const std::string& TimetableManager::getStopNameFromStopId(std::string const& aStopId) const
  {
    return data->stops.at(aStopId).stopName;
  }

  const std::string& TimetableManager::getStopIdFromStopName(std::string const& aStopName) const
  {
    return std::ranges::find_if(data->stops,
                                [&aStopName](const auto& stop) { return stop.second.stopName == aStopName; })
      ->first;
  }

  std::vector<std::string> TimetableManager::getStopIdsFromStopName(std::string const& aStopName) const
  {
#if defined(__cpp_lib_ranges_to_container)
    return data->stops | std::views::filter([&aStopName](const auto& stop) { return stop.second.stopName == aStopName; })
           | std::views::keys | std::ranges::to<std::vector<std::string>>();
#else
    std::vector<std::string> stopNames;
    auto filteredStops = data->stops
                        | std::views::filter([&aStopName](const auto& stop) { return stop.second.stopName == aStopName; });
    std::ranges::transform(filteredStops, std::back_inserter(stopNames), [](const auto& stop) {
      return stop.first;
    });
    return stopNames;
#endif
  }
  const schedule::gtfs::StopTime& TimetableManager::getStopTimeFromStopId(std::string const& aStopId) const
  {
    return data->stopTimes.at(aStopId).front();
  }

  std::vector<schedule::gtfs::Stop> TimetableManager::getAllStopsOnTrip(std::string const& aTripId) const
  {
    // Retrieve all StopTimes for the given tripId, sorted by departure time
#if defined(__cpp_lib_ranges_to_container)
    auto stopTimesMatchingTripId
      = data->stopTimes | std::views::values | std::views::join
        | std::views::filter([&](const schedule::gtfs::StopTime& stopTime) { return stopTime.tripId == aTripId; })
        | std::ranges::to<std::vector<schedule::gtfs::StopTime>>();
#else
    std::vector<schedule::gtfs::StopTime> stopTimesMatchingTripId;

    // Filter and transform the values into a vector
    auto filteredStopTimes = data->stopTimes
                             | std::views::values // Extract values from the map
                             | std::views::join   // Flatten the range of ranges
                             | std::views::filter([&](const schedule::gtfs::StopTime& stopTime) {
                                 return stopTime.tripId == aTripId;
                               });

    std::ranges::transform(filteredStopTimes, std::back_inserter(stopTimesMatchingTripId), [](const auto& stopTime) {
      return stopTime;
    });
#endif


    std::ranges::sort(stopTimesMatchingTripId,
                      [](const schedule::gtfs::StopTime& a, const schedule::gtfs::StopTime& b) { return a.departureTime < b.departureTime; });

    std::vector<schedule::gtfs::Stop> stopsForTrip;
    for (const auto& stopTime : stopTimesMatchingTripId) {
      stopsForTrip.push_back(data->stops.at(stopTime.stopId));
    }

    return stopsForTrip;
  }

  const std::vector<schedule::gtfs::StopTime>& TimetableManager::getStopTimesFromStopId(std::string const& aStopId) const
  {
    return data->stopTimes.at(aStopId);
  }

  schedule::gtfs::Route TimetableManager::getRouteFromTripId(std::string const& aTripId) const
  {
    const auto& trip = data->trips.at(aTripId);
    return data->routes.at(trip.routeId);
  }

  std::vector<schedule::gtfs::StopTime> TimetableManager::getStopTimesFromStopIdStartingFromSpecificTime(std::string const& aStopId, unsigned int secondsGreaterThan) const
  {
#if defined(__cpp_lib_ranges_to_container)
    return data->stopTimes.at(aStopId)
           | std::views::filter([secondsGreaterThan, this](const schedule::gtfs::StopTime& stopTime) {
               return stopTime.departureTime.toSeconds() > secondsGreaterThan;
             })
           | std::ranges::to<std::vector<schedule::gtfs::StopTime>>();
#else
    std::vector<schedule::gtfs::StopTime> stopTimesFiltered;
    auto departureTimeGreaterThan = [secondsGreaterThan](const schedule::gtfs::StopTime& stopTime) {
      return stopTime.departureTime.toSeconds() > secondsGreaterThan;
    };
    std::ranges::transform(data->stopTimes.at(aStopId) | std::views::filter(departureTimeGreaterThan), std::back_inserter(stopTimesFiltered), [secondsGreaterThan](const schedule::gtfs::StopTime& stopTime) {
        return stopTime;
    });
    std::erase_if(stopTimesFiltered, [](const schedule::gtfs::StopTime& stopTime) {
      return stopTime.tripId.empty();
    });
    return stopTimesFiltered;
#endif
  }

  bool TimetableManager::isServiceActiveOnDay(std::string const& aServiceId, const std::chrono::weekday aDay) const
  {
    return data->calendars.at(aServiceId).weekdayService.contains(aDay);
  }

  const schedule::gtfs::Trip& TimetableManager::getTripsFromStopTimeTripId(std::string const& aTripId) const
  {
    return data->trips.at(aTripId);
  }

} // gtfs
