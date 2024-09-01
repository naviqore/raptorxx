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
    sortTripStopTimes();
  }

  void TimetableManager::buildTripsToRoutesRelations() const
  {
    std::ranges::for_each(data->trips | std::views::values, [this](const schedule::gtfs::Trip& trip) {
      data->routes.at(trip.routeId).trips.push_back(trip.tripId);
    });
  }

  void TimetableManager::buildStopTimesToTripsAndRoutesRelations() const
  {
    for (const auto& stopTime : data->stopTimes | std::views::values | std::views::join) {
      schedule::gtfs::Trip& tripServingStopTime = data->trips.at(stopTime.tripId);
      tripServingStopTime.stopTimes.insert(stopTime);
    }
  }

  void TimetableManager::sortTripStopTimes() const
  {
    // std::ranges::for_each(data->trips | std::views::values, [this](schedule::gtfs::Trip& trip) {
    //   std::ranges::sort(trip.stopTimes, schedule::gtfs::stopTimeCompare);
    // });
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

  std::vector<schedule::gtfs::Route> TimetableManager::getRoutes() const
  {
#if defined(__cpp_lib_ranges_to_container)
    return data->routes | std::views::values | std::ranges::to<std::vector<schedule::gtfs::Route>>();
#else
    std::vector<schedule::gtfs::Route> routes;
    std::ranges::transform(data->routes, std::back_inserter(routes), [](const auto& route) {
      return route.second;
    });
    return routes;
#endif
  }

} // gtfs
