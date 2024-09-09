//
// Created by MichaelBrunner on 19/07/2024.
//

#include "SubRoute.h"
#include "RoutePartitioner.h"

#include <execution>
#include <functional>
#include <ranges>


namespace converter {

  RoutePartitioner::RoutePartitioner(schedule::gtfs::GtfsData* data)
    : data(data)
  {
    std::ranges::for_each(data->routes | std::views::values, [this](schedule::gtfs::Route const& route) {
      this->processRoute(route);
    });
  }

  std::vector<SubRoute> RoutePartitioner::getSubRoutes(std::string const& routeId) const
  {
    const auto& subRoutesForRoute = subRoutes.at(routeId);
#if defined(__cpp_lib_ranges_to_container)
    return std::ranges::views::values(subRoutesForRoute) | std::ranges::to<std::vector<SubRoute>>();
#else
    std::vector<SubRoute> subRoutesVector;
    std::ranges::transform(subRoutesForRoute, std::back_inserter(subRoutesVector), [](const auto& pair) {
      return pair.second;
    });
    return subRoutesVector;
#endif
  }

  const SubRoute& RoutePartitioner::getSubRoute(std::string const& tripId) const
  {
    const auto& trip = data->trips.at(tripId);
    //const auto route = data->routes.at(trip.routeId);
    const auto& subRoutesForRoute = subRoutes.at(trip.routeId);
    const auto key = generateStopSequenceKey(tripId);
    if (!subRoutesForRoute.contains(key)) {
      throw std::invalid_argument("SubRoute for trip " + tripId + " not found");
    }
    return subRoutesForRoute.at(key);
  }

  const std::vector<std::string>& RoutePartitioner::getTrips(std::string const& routeId) const
  {
    return data->routes.at(routeId).trips;
  }

  void RoutePartitioner::processRoute(schedule::gtfs::Route const& route)
  {
    std::unordered_map<std::string, SubRoute> sequenceKeyToSubRouteHashMap;
    sequenceKeyToSubRouteHashMap.reserve(getTrips(route.routeId).size());

    for (const auto& tripId : getTrips(route.routeId)) {
      const schedule::gtfs::Trip& trip = data->trips.at(tripId);
      auto key = this->generateStopSequenceKey(tripId);

      auto [it, inserted] = sequenceKeyToSubRouteHashMap.try_emplace(key,
                                                                     SubRoute(std::format("{}_sr{}", route.routeId, sequenceKeyToSubRouteHashMap.size() + 1),
                                                                              route.routeId,
                                                                              key,
                                                                              extractStopSequence(trip)));
      if (!inserted) {
        it->second.addTrip(trip);
      }
    }
    subRoutes.insert_or_assign(route.routeId, std::move(sequenceKeyToSubRouteHashMap));
  }

  std::string RoutePartitioner::generateStopSequenceKey(const std::string& tripId) const
  {
    const auto range = data->trips.at(tripId).stopTimes;
    std::string sequenceKey;
    sequenceKey.reserve(std::distance(range.begin(), range.end()) * 401); // 400 + 1 for "_"

    bool first = true;
    for (auto it = range.begin(); it != range.end(); ++it) {
      if (!first) {
        sequenceKey += "_";
      }
      else {
        first = false;
      }
      sequenceKey += (*it)->stopId;
    }
    return sequenceKey;
  }

  std::vector<schedule::gtfs::Stop> RoutePartitioner::extractStopSequence(schedule::gtfs::Trip const& aTrip) const
  {
    std::vector<schedule::gtfs::Stop> stops;

    // const auto& range = data->trips.at(aTrip.tripId).stopTimes;
    const auto& range = aTrip.stopTimes;
    stops.reserve(std::distance(range.begin(), range.end()));

    std::ranges::transform(range, std::back_inserter(stops), [&](const schedule::gtfs::StopTime* stopTime) {
      return data->stops.at(stopTime->stopId);
    });

    return stops;
  }

} // gtfs
  // schedule