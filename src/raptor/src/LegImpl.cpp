//
// Created by MichaelBrunner on 23/06/2024.
//

#include "LegImpl.h"

namespace raptor {

  LegImpl::LegImpl(std::string routeId, std::string tripId, std::string fromStopId, std::string toStopId, const types::raptorIdx departureTime, const types::raptorIdx arrivalTime, Type type)
    : routeId(std::move(routeId))
    , tripId(std::move(tripId))
    , fromStopId(std::move(fromStopId))
    , toStopId(std::move(toStopId))
    , departureTime(departureTime)
    , arrivalTime(arrivalTime)
    , type(type) {
  }
  std::string const& LegImpl::getRouteId() const {
    return routeId;
  }
  std::string const& LegImpl::getTripId() const {
    return tripId;
  }
  std::string const& LegImpl::getFromStopId() const {
    return fromStopId;
  }
  std::string const& LegImpl::getToStopId() const {
    return toStopId;
  }
  int LegImpl::getDepartureTime() const {
    return departureTime;
  }
  int LegImpl::getArrivalTime() const {
    return arrivalTime;
  }
  std::optional<Leg::Type> LegImpl::getType() const {
    return type;
  }
} // raptor