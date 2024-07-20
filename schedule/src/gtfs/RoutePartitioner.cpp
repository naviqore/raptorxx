//
// Created by MichaelBrunner on 19/07/2024.
//

#include "RoutePartitioner.h"

#include <execution>
#include <functional>
#include <ranges>


namespace schedule::gtfs {
  RoutePartitioner::RoutePartitioner(GtfsData* data)
    : data(data) {

    // auto routes = data->routes | std::views::values;
    // std::vector<Route> routeList(routes.begin(), routes.end());
    // std::for_each(std::execution::par, routeList.begin(), routeList.end(), [this](Route const& route) {
    //   this->processRoute(route);
    // });

    std::ranges::for_each(data->routes | std::views::values, [this](Route const& route) {
      this->processRoute(route);
    });
  }

  std::vector<SubRoute> RoutePartitioner::getSubRoutes(std::string const& routeId) const {
    const auto& subRoutesForRoute = subRoutes.at(routeId);
    return std::ranges::views::values(subRoutesForRoute) | std::ranges::to<std::vector<SubRoute>>();
  }

  const SubRoute& RoutePartitioner::getSubRoute(std::string const& tripId) const {
    const auto route = data->routes.at(tripId);
    const auto& subRoutesForRoute = subRoutes.at(route.routeId);
    const auto key = generateStopSequenceKey(tripId);
    return subRoutesForRoute.at(key);
  }

  void RoutePartitioner::processRoute(Route const& route) {
    std::unordered_map<std::string, SubRoute> sequenceKeyToSubRoute;
    sequenceKeyToSubRoute.reserve(data->routes.at(route.routeId).trips.size());

    for (const auto& tripId : data->routes.at(route.routeId).trips)
    {
      const Trip& trip = data->trips.at(tripId);
      auto key = this->generateStopSequenceKey(tripId);

      auto [it, inserted] = sequenceKeyToSubRoute.try_emplace(key,
                                                              SubRoute(std::format("{}_sr{}", route.routeId, sequenceKeyToSubRoute.size() + 1), route.routeId, key, extractStopSequence(trip)));
      if (!inserted)
      {
        it->second.addTrip(trip);
      }
    }
    subRoutes.insert_or_assign(route.routeId, std::move(sequenceKeyToSubRoute));
  }


  // void RoutePartitioner::processRoute(Route const& route) {
  //   std::unordered_map<std::string, SubRoute> sequenceKeyToSubRoute{};
  //   std::vector<Trip> trips;
  //   std::ranges::transform(data->routes.at(route.routeId).trips, std::back_inserter(trips), [&](std::string const& tripId) {
  //     return data->trips.at(tripId);
  //   });

  //   std::ranges::for_each(trips, [&, this](Trip const& trip) {
  //     auto key = this->generateStopSequenceKey(trip);
  //     if (!sequenceKeyToSubRoute.contains(key))
  //     {
  //       auto subRouteId = std::format("{}_sr{}", route.routeId, sequenceKeyToSubRoute.size() + 1);
  //       sequenceKeyToSubRoute.insert_or_assign(key, SubRoute(std::move(subRouteId), route.routeId, std::move(key), std::move(extractStopSequence(trip))));
  //     }
  //     sequenceKeyToSubRoute.at(key).addTrip(trip);
  //   });
  //   subRoutes.insert_or_assign(route.routeId, sequenceKeyToSubRoute);
  // }

  std::string RoutePartitioner::generateStopSequenceKey(const std::string& tripId) const {
    const auto range = data->trips.at(tripId).stopTimes;
    std::string sequenceKey;
    sequenceKey.reserve(std::distance(range.begin(), range.end()) * 401); // 400 + 1 for "_"

    bool first = true;
    for (auto it = range.begin(); it != range.end(); ++it)
    {
      if (!first)
      {
        sequenceKey += "_";
      }
      else
      {
        first = false;
      }
      sequenceKey += (*it)->stopId;
    }
    return sequenceKey;
  }


  // std::string RoutePartitioner::generateStopSequenceKey(const Trip& trip) const {
  //   std::string sequenceKey;
  //
  //   auto stopTimesForTripId = this->data->stopTimes
  //                             | std::views::values
  //                             | std::views::join
  //                             | std::views::filter([&trip](const StopTime& stopTime) {
  //                                 return stopTime.tripId == trip.tripId;
  //                               });
  //
  //   for (const auto& stopTime : stopTimesForTripId)
  //   {
  //     if (!sequenceKey.empty())
  //     {
  //       sequenceKey += "_";
  //     }
  //     sequenceKey += stopTime.stopId;
  //   }
  //   return sequenceKey;
  // }

  std::vector<Stop> RoutePartitioner::extractStopSequence(Trip const& aTrip) const {
    std::vector<Stop> stops;
    const auto range = data->trips.at(aTrip.tripId).stopTimes;
    stops.reserve(std::distance(range.begin(), range.end()));

    for (auto it = range.begin(); it != range.end(); ++it)
    {
      stops.push_back(data->stops.at((*it)->stopId));
    }
    return stops;
  }


  /*std::vector<Stop> RoutePartitioner::extractStopSequence(Trip const& aTrip) const {
    return data->stopTimes | std::views::values | std::views::join
           | std::views::filter([&](const StopTime& stopTime) { return stopTime.tripId == aTrip.tripId; })
           | std::views::transform([&](const StopTime& stopTime) { return data->stops.at(stopTime.stopId); })
           | std::ranges::to<std::vector<Stop>>();
  }*/
} // gtfs
  // schedule