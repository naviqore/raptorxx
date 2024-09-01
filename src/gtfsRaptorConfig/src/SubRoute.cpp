//
// Created by MichaelBrunner on 02/08/2024.
//

#include "SubRoute.h"

namespace converter {

  SubRoute::SubRoute(std::string&& subRouteId, std::string routeId, std::string stopSequenceKey, std::vector<schedule::gtfs::Stop>&& stopsSequence)
    : SubRouteId(std::move(subRouteId))
    , routeId(std::move(routeId))
    , stopSequenceKey(std::move(stopSequenceKey))
    , stopsSequence(std::move(stopsSequence))
  {
    if (this->SubRouteId.empty()) {
      throw std::invalid_argument("SubRouteId must not be empty");
    }
    if (this->routeId.empty()) {
      throw std::invalid_argument("routeId must not be empty");
    }
  }

  SubRoute::SubRoute(const SubRoute& aSubRoute)
    : SubRouteId(aSubRoute.SubRouteId)
    , routeId(aSubRoute.routeId)
    , stopSequenceKey(aSubRoute.stopSequenceKey)
    , stopsSequence(aSubRoute.stopsSequence)
    , trips(aSubRoute.trips)
  {
  }

  void SubRoute::addTrip(schedule::gtfs::Trip const& trip)
  {
    this->trips.push_back(trip);
  }

  const std::string& SubRoute::getSubRouteId() const
  {
    return this->SubRouteId;
  }

  const std::string& SubRoute::getRouteId() const
  {
    return this->routeId;
  }

  const std::string& SubRoute::getStopSequenceKey() const
  {
    return stopSequenceKey;
  }

  const std::vector<schedule::gtfs::Stop>& SubRoute::getStopsSequence() const
  {
    return this->stopsSequence;
  }

  size_t SubRoute::stopIndex(const std::string_view stopId) const
  {
    for (size_t i = 0; i < this->stopsSequence.size(); ++i) {
      if (this->stopsSequence[i].stopId == stopId) {
        return i;
      }
    }
    return this->stopsSequence.size();
  }

  const std::vector<schedule::gtfs::Trip>& SubRoute::getTrips() const
  {
    return this->trips;
  }

  bool SubRoute::operator==(const SubRoute& aSubRoute) const
  {
    return this->SubRouteId == aSubRoute.getSubRouteId(); // && stopSequenceKey == aSubRoute.stopSequenceKey
  }
} // converter