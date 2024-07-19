//
// Created by MichaelBrunner on 19/07/2024.
//

#ifndef ROUTEPARTITIONER_H
#define ROUTEPARTITIONER_H

#include "GtfsData.h"
#include "model/Route.h"

#include <functional>
#include <map>
#include <utility>
#include <schedule_export.h>

namespace schedule::gtfs {

  //TODO  header file and source file
  class SCHEDULE_API SubRoute
  {
    std::string SubRouteId{};
    std::string routeId{};
    std::string stopSequenceKey{};
    std::vector<Stop> stopsSequence{};
    std::vector<Trip> trips{};

  public:
    explicit SubRoute(std::string&& subRouteId, std::string const& routeId, std::string const& stopSequenceKey, std::vector<Stop>&& stopsSequence)
      : SubRouteId(std::move(subRouteId))
      , routeId(routeId)
      , stopSequenceKey(stopSequenceKey)
      , stopsSequence(std::move(stopsSequence)) {
    }

    SubRoute(const SubRoute& aSubRoute)
      : SubRouteId(aSubRoute.SubRouteId)
      , stopSequenceKey(aSubRoute.stopSequenceKey)
      , stopsSequence(aSubRoute.stopsSequence)
      , trips(aSubRoute.trips) {
    }

    SubRoute(SubRoute&& aSubRoute) noexcept = default;
    SubRoute& operator=(const SubRoute& aSubRoute) = default;
    SubRoute& operator=(SubRoute&& aSubRoute) noexcept = default;

    void addTrip(Trip const& trip) {
      trips.push_back(trip);
    }

    [[nodiscard]] const std::string& getSubRouteId() const {
      return SubRouteId;
    }

    [[nodiscard]] const std::string& getRouteId() const {
      return routeId;
    }

    [[nodiscard]] const std::string& getStopSequenceKey() const {
      return stopSequenceKey;
    }

    [[nodiscard]] const std::vector<Stop>& getStopsSequence() const {
      return stopsSequence;
    }

    [[nodiscard]] size_t stopIndex(const std::string_view stopId) const {
      for (size_t i = 0; i < stopsSequence.size(); ++i)
      {
        if (stopsSequence[i].stopId == stopId)
        {
          return i;
        }
      }
      return stopsSequence.size();
    }

    [[nodiscard]] const std::vector<Trip>& getTrips() const {
      return trips;
    }

    bool operator==(const SubRoute& aSubRoute) const {
      return SubRouteId == aSubRoute.getSubRouteId(); // && stopSequenceKey == aSubRoute.stopSequenceKey
    }

    std::function<size_t(const SubRoute&)> SubRouteHash = [](const SubRoute& aSubRoute) {
      return std::hash<std::string>{}(aSubRoute.getSubRouteId());
    };

    std::function<bool(const SubRoute&, const SubRoute&)> SubRouteEqual = [](const SubRoute& aSubRoute, const SubRoute& anotherSubRoute) {
      return aSubRoute == anotherSubRoute;
    };
  };


  class SCHEDULE_API RoutePartitioner
  {

  public:
    explicit RoutePartitioner(GtfsData* data);

    [[nodiscard]] std::vector<SubRoute> getSubRoutes(std::string const& routeId) const;

    [[nodiscard]] const SubRoute& getSubRoute(std::string const& tripId) const;


  private:
    std::unordered_map<std::string, std::unordered_map<std::string, SubRoute> /*, decltype(routeHash), decltype(routeEqual)*/> subRoutes{};
    GtfsData* data;

    void processRoute(Route const& route);

    [[nodiscard]] std::string generateStopSequenceKey(const std::string& tripId) const;

    [[nodiscard]] std::vector<Stop> extractStopSequence(Trip const& aTrip) const;
  };

} // gtfs
// schedule

#endif //ROUTEPARTITIONER_H
