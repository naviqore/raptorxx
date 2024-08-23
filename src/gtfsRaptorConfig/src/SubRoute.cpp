//
// Created by MichaelBrunner on 02/08/2024.
//

#include "SubRoute.h"

namespace converter {

  SubRoute::SubRoute(std::string&& subRouteId, std::string routeId, std::string stopSequenceKey, std::vector<schedule::gtfs::Stop>&& stopsSequence)
    : SubRouteId(std::move(subRouteId))
    , routeId(std::move(routeId))
    , stopSequenceKey(std::move(stopSequenceKey))
    , stopsSequence(std::move(stopsSequence)) {
  }

  SubRoute::SubRoute(const SubRoute& aSubRoute)
    : SubRouteId(aSubRoute.SubRouteId)
    , stopSequenceKey(aSubRoute.stopSequenceKey)
    , stopsSequence(aSubRoute.stopsSequence)
    , trips(aSubRoute.trips) {
  }

  void SubRoute::addTrip(schedule::gtfs::Trip const& trip) {
    trips.push_back(trip);
  }

  const std::string& SubRoute::getSubRouteId() const {
    return SubRouteId;
  }

  const std::string& SubRoute::getRouteId() const {
    return routeId;
  }

  const std::vector<schedule::gtfs::Stop>& SubRoute::getStopsSequence() const {
    return stopsSequence;
  }

  size_t SubRoute::stopIndex(const std::string_view stopId) const {
    for (size_t i = 0; i < stopsSequence.size(); ++i)
    {
      if (stopsSequence[i].stopId == stopId)
      {
        return i;
      }
    }
    return stopsSequence.size();
  }

  const std::vector<schedule::gtfs::Trip>& SubRoute::getTrips() const {
    return trips;
  }

  bool SubRoute::operator==(const SubRoute& aSubRoute) const {
    return SubRouteId == aSubRoute.getSubRouteId(); // && stopSequenceKey == aSubRoute.stopSequenceKey
  }
} // converter