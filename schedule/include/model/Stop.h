//
// Created by MichaelBrunner on 02/06/2024.
//

#ifndef STOP_H
#define STOP_H

#include <spatial/Coordinate.h>
#include <spatial/CoordinateComponent.h>


#include <string>
#include <stdexcept>

// https://gtfs.org/schedule/reference/#stopstxt
// Required

namespace schedule::gtfs {
  struct Stop
  {
    // TODO consider float instead of double for lat/lon
    Stop(std::string&& aStopId, std::string&& aStopName, const geometry::Coordinate<double> aStopLat, const geometry::Coordinate<double> aStopLon, std::string&& aParentStation)
      : stopId(std::move(aStopId))
      , stopName(std::move(aStopName))
      , stopPoint(aStopLat, aStopLon)
      , parentStation(std::move(aParentStation)) {
      if (stopId.empty()
          || stopName.empty())
      {
        throw std::invalid_argument("Mandatory stop fields must not be empty");
      }
    }
    std::string stopId;
    std::string stopName;
    geometry::CoordinateComponent<geometry::Coordinate<double>> stopPoint;
    std::string parentStation;

    //TODO in future create another class to handle the raptor specific stuff - keep this class clean for GTFS
    bool visited = false;
    std::vector<std::string> routesServedByStop{};

    // This vector, arrTimesKTrips, tracks the minimum arrival times at this stop for varying numbers of allowed trips.
    // Specifically, the element at index k (0-based) stores the earliest time one can arrive at this stop by using up to k+1 trips.
    // It enables efficient computation of optimal arrival times under increasing trip constraints, facilitating dynamic routing optimizations.
    std::vector<size_t> arrivalTimesKTrips;
    unsigned int earliestArrivalTime = std::numeric_limits<unsigned int>::max();
    std::string earliestTripId{};
    std::string transferFromStopId{};
    std::unordered_map<size_t, std::string> earliestTripIdKTrips{};
  };

  inline auto stopHash = [](const Stop& stop) {
    return std::hash<std::string>{}(stop.stopId);
  };

  inline auto stopEqual = [](const Stop& lhs, const Stop& rhs) {
    return lhs.stopId == rhs.stopId;
  };

} // gtfs

#endif //STOP_H