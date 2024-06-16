//
// Created by MichaelBrunner on 02/06/2024.
//

#ifndef STOP_TIME_H
#define STOP_TIME_H

#include "utils/ServiceDayTime.h"


#include <string>
#include <stdexcept>

namespace gtfs {
  struct StopTime
  {
    StopTime(std::string&& aTripId, std::string&& aArrivalTime, std::string&& aDepartureTime, std::string&& aStopId, int const aStopSequence)
      : tripId(std::move(aTripId))
      , arrivalTime(utils::ServiceDayTime::fromString(std::move(aArrivalTime)))
      , departureTime(utils::ServiceDayTime::fromString(std::move(aDepartureTime)))
      , stopId(std::move(aStopId))
      , stopSequence(aStopSequence) {
      if (tripId.empty())
      {
        throw std::invalid_argument("Mandatory stop time fields must not be empty");
      }
    }
    std::string tripId;
    utils::ServiceDayTime arrivalTime;
    utils::ServiceDayTime departureTime;
    std::string stopId;
    int stopSequence;
  };

} // gtfs

#endif //STOP_TIME_H