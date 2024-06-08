//
// Created by MichaelBrunner on 02/06/2024.
//

#ifndef STOP_H
#define STOP_H

#include <Coordinate.h>
#include <CoordinateComponent.h>


#include <string>
#include <stdexcept>

// https://gtfs.org/schedule/reference/#stopstxt
// Required

namespace gtfs {
  struct Stop
  {
    // TODO consider float instead of double for lat/lon
    Stop(std::string&& aStopId, std::string&& aStopName, const geometry::Coordinate<double> aStopLat, const geometry::Coordinate<double> aStopLon)
      : stopId(std::move(aStopId))
      , stopName(std::move(aStopName))
      , stopPoint(aStopLat, aStopLon) {
      if (stopId.empty()
          || stopName.empty())
      {
        throw std::invalid_argument("Mandatory stop fields must not be empty");
      }
    }
    std::string stopId;
    std::string stopName;
    geometry::CoordinateComponent<geometry::Coordinate<double>> stopPoint;
  };

} // gtfs

#endif //STOP_H