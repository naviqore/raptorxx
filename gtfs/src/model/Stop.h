//
// Created by MichaelBrunner on 02/06/2024.
//

#ifndef STOP_H
#define STOP_H

#include <string>
#include <stdexcept>

// https://gtfs.org/schedule/reference/#stopstxt
// Required

namespace gtfs {
  struct Stop
  {
    Stop(std::string&& aStopId, std::string&& aStopName, const double aStopLat, const double aStopLon)
      : stopId(std::move(aStopId))
      , stopName(std::move(aStopName))
      , stopLat(aStopLat)
      , stopLon(aStopLon) {
      if (stopId.empty()
          || stopName.empty())
      {
        throw std::invalid_argument("Mandatory stop fields must not be empty");
      }
    }
    std::string stopId;
    std::string stopName;
    double stopLat;
    double stopLon;
  };

} // gtfs

#endif //STOP_H