//
// Created by MichaelBrunner on 02/06/2024.
//

#ifndef TRIP_H
#define TRIP_H

#include <string>
#include <stdexcept>

namespace gtfs {
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

} // gtfs

#endif //TRIP_H
