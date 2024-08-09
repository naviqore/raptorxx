//
// Created by MichaelBrunner on 26/07/2024.
//

#include "RouteContainer.h"

#include <utility>

namespace raptor {

  RouteContainer::RouteContainer(std::string id, const std::map<int, std::string>& stopSequence, const std::map<std::string, std::vector<StopTime>>& trips)
    : routeContainerId(std::move(id))
    , StopSequenceMap(stopSequence)
    , idTripsMap(trips) {}

  const std::string& RouteContainer::id() const {
    return routeContainerId;
  }

  bool RouteContainer::operator<(const RouteContainer& other) const {
    const StopTime& thisFirstStopTime = this->idTripsMap.begin()->second[0];
    const StopTime& otherFirstStopTime = other.idTripsMap.begin()->second[0];

    return thisFirstStopTime.departure < otherFirstStopTime.departure;
  }

  const std::map<int, std::string>& RouteContainer::stopSequence() const {
    return StopSequenceMap;
  }

  const std::map<std::string, std::vector<StopTime>>& RouteContainer::trips() const {
    return idTripsMap;
  }
} // raptor