//
// Created by MichaelBrunner on 02/06/2024.
//

#ifndef TRIP_H
#define TRIP_H

#include "StopTime.h"


#include <algorithm>
#include <string>
#include <stdexcept>

namespace schedule::gtfs {
  struct StopTime;
}
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
    std::optional<std::string> boardingStopId{};
    // TODO dangling pointer dangerous.... but shared is not performant
    std::vector<const StopTime*> stopTimes{};

    // arrival times at the stops of this trip
    std::vector<size_t> arrivalTimesAtStops{};
    std::vector<size_t> departureTimesAtStops{};

    void addTimesAtStop(size_t const arrivalTime, size_t const departureTime) {
      arrivalTimesAtStops.push_back(arrivalTime);
      departureTimesAtStops.push_back(departureTime);
    }

    // std::set<StopTime, decltype(stopTimeLessByStopSequence)> stopTimes{}; // maybe use set and order by stopSequence

    // [[nodiscard]] std::vector<StopTime> getStopTimesSorted() const {
    //   auto items = std::vector<StopTime>(stopTimes.begin(), stopTimes.end());
    //   std::ranges::sort(items, [](const StopTime& a, const StopTime& b) {
    //     return a.departureTime < b.departureTime;
    //   });
    //   return items;
    // }
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
