//
// Created by MichaelBrunner on 02/06/2024.
//

#ifndef TRIP_H
#define TRIP_H

#include "StopTime.h"

#include <vector>
#include <algorithm>
#include <string>
#include <stdexcept>
#include <schedule_export.h>

namespace schedule::gtfs {
  struct StopTime;
}
namespace schedule::gtfs {

  inline auto stopTimeCompare = [](const StopTime* lhs, const StopTime* rhs) {
    return lhs->departureTime.toSeconds() < rhs->departureTime.toSeconds();
  };

  struct SCHEDULE_API Trip {
    Trip(std::string&& aRouteId, std::string&& aServiceId, std::string&& aTripId)
      : routeId(std::move(aRouteId))
      , serviceId(std::move(aServiceId))
      , tripId(std::move(aTripId))
    {
      if (routeId.empty()
          || serviceId.empty()
          || tripId.empty()) {
        throw std::invalid_argument("Mandatory trip fields must not be empty");
      }
    }
    std::string routeId;
    std::string serviceId;
    std::string tripId;

    std::set<const StopTime*, decltype(stopTimeCompare)> stopTimes{};
    // std::vector<StopTime> stopTimes{}; //TODO: think about storing pointers to StopTime objects
  };


  inline auto tripHash = [](const Trip& trip) {
    const auto h1 = std::hash<std::string>{}(trip.tripId);
    const auto h2 = std::hash<std::string>{}(trip.routeId);
    return h1 ^ (h2 << 1);
  };

  inline auto tripEqual = [](const Trip& lhs, const Trip& rhs) {
    return lhs.tripId == rhs.tripId && lhs.routeId == rhs.routeId;
  };

} // gtfs

#endif //TRIP_H
