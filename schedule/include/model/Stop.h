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
  };

  inline auto stopHash = [](const Stop& stop) {
    return std::hash<std::string>{}(stop.stopId);
  };

  inline auto stopEqual = [](const Stop& lhs, const Stop& rhs) {
    return lhs.stopId == rhs.stopId;
  };

} // gtfs

#endif //STOP_H