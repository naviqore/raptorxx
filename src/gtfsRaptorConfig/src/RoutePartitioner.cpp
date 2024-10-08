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
    processActiveRoutes();
  }

  void RoutePartitioner::processActiveRoutes()
  {
    std::ranges::for_each(data->routes | std::views::values, [this](const std::shared_ptr<schedule::gtfs::Route>& route) {
      if (route->isRouteActive && !processedRoutes.contains(route->routeId)) {
        this->processRoute(*route);
        processedRoutes.insert(route->routeId);
      }
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
    const auto trip = data->getTrip(tripId);
    const auto& subRoutesForRoute = subRoutes.at(trip->routeId);
    const auto key = generateStopSequenceKey(tripId);
    if (!subRoutesForRoute.contains(key)) {
      throw std::invalid_argument("SubRoute for trip " + tripId + " not found");
    }
    return subRoutesForRoute.at(key);
  }

  const std::unordered_set<std::string>& RoutePartitioner::getTrips(std::string const& routeId) const
  {
    return data->getRoute(routeId)->trips;
  }

  void RoutePartitioner::processRoute(schedule::gtfs::Route const& route)
  {
    std::unordered_map<std::string, SubRoute> sequenceKeyToSubRouteHashMap;
    sequenceKeyToSubRouteHashMap.reserve(getTrips(route.routeId).size());

    for (const auto& tripId : getTrips(route.routeId)) {
      const auto trip = data->getTrip(tripId);
      if (!trip->isTripActive) {
        continue;
      }
      auto key = this->generateStopSequenceKey(tripId);

      auto [it, inserted] = sequenceKeyToSubRouteHashMap.try_emplace(key,
                                                                     SubRoute(std::format("{}_sr{}", route.routeId, sequenceKeyToSubRouteHashMap.size() + 1),
                                                                              route.routeId,
                                                                              key,
                                                                              extractStopSequence(*trip)));
      if (!inserted) {
        it->second.addTrip(*trip);
      }
    }
    subRoutes.insert_or_assign(route.routeId, std::move(sequenceKeyToSubRouteHashMap));
  }

  std::string RoutePartitioner::generateStopSequenceKey(const std::string& tripId) const
  {
    const auto& stopTimesRange = data->getTrip(tripId)->stopTimes;
    std::string sequenceKey;
    sequenceKey.reserve(std::distance(stopTimesRange.begin(), stopTimesRange.end()) * 401); // 400 + 1 for "_"

    bool first = true;
    for (const auto stopItem : stopTimesRange) {
      if (!first) {
        sequenceKey += "_";
      }
      else {
        first = false;
      }
      sequenceKey += stopItem->stopId;
    }
    return sequenceKey;
  }

  std::vector<const schedule::gtfs::Stop*> RoutePartitioner::extractStopSequence(schedule::gtfs::Trip const& aTrip) const
  {
    std::vector<const schedule::gtfs::Stop*> stops;

    const auto& range = aTrip.stopTimes;
    stops.reserve(std::distance(range.begin(), range.end()));

    std::ranges::transform(range, std::back_inserter(stops), [&](const schedule::gtfs::StopTime* stopTime) {
      return data->getStop(stopTime->stopId);
    });

    return stops;
  }

} // gtfs
// schedule
