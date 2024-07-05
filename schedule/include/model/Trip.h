//
// Created by MichaelBrunner on 02/06/2024.
//

#ifndef TRIP_H
#define TRIP_H

#include <string>
#include <stdexcept>

namespace schedule::gtfs {
  struct Trip
  {
    Trip(std::string&& aRouteId, std::string&& aServiceId, std::string&& aTripId)
      : routeId(std::move(aRouteId))
      , serviceId(std::move(aServiceId))
      , tripId(std::move(aTripId)) {
      if (routeId.empty()
          || serviceId.empty()
          || tripId.empty())
      {
        throw std::invalid_argument("Mandatory trip fields must not be empty");
      }
    }
    std::string routeId;
    std::string serviceId;
    std::string tripId;
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
