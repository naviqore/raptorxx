//
// Created by MichaelBrunner on 02/06/2024.
//

#ifndef STOP_TIME_H
#define STOP_TIME_H

#include "ServiceDayTime.h"
#include <string>
#include <stdexcept>
#include <schedule_export.h>

namespace schedule::gtfs {
  struct SCHEDULE_API StopTime {
    StopTime(std::string&& aTripId, std::string&& aArrivalTime, std::string&& aDepartureTime, std::string aStopId, int const aStopSequence)
      : tripId(std::move(aTripId))
      , arrivalTime(utils::ServiceDayTime::fromString(std::move(aArrivalTime)))
      , departureTime(utils::ServiceDayTime::fromString(std::move(aDepartureTime)))
      , stopId(std::move(aStopId))
      , stopSequence(aStopSequence)
    {
      if (tripId.empty()) {
        throw std::invalid_argument("Mandatory stop time fields must not be empty");
      }
    }
    std::string tripId{};
    utils::ServiceDayTime arrivalTime = utils::ServiceDayTime::fromSeconds(utils::ServiceDayTime::Second(0));
    utils::ServiceDayTime departureTime = utils::ServiceDayTime::fromSeconds(utils::ServiceDayTime::Second(0));
    std::string stopId{};
    int stopSequence{};
  };

  inline auto stopTimeHash = [](StopTime const& stopTime) noexcept {
    return std::hash<std::string>{}(stopTime.stopId);
  };

  inline auto stopTimeEqual = [](StopTime const& lhs, StopTime const& rhs) noexcept {
    return lhs.stopId == rhs.stopId;
  };

  inline auto stopTimeLessByStopSequence = [](StopTime const& lhs, StopTime const& rhs) noexcept {
    return lhs.stopSequence < rhs.stopSequence;
  };

} // gtfs

#endif //STOP_TIME_H
