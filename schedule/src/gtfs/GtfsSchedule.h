//
// Created by MichaelBrunner on 16/06/2024.
//

#ifndef GTFSSCHEDULE_H
#define GTFSSCHEDULE_H
#include "RelationManager.h"

namespace gtfs {

class GtfsSchedule {

  RelationManager relationManager;

public:
  explicit GtfsSchedule(const GtfsData& data);
  // https://en.cppreference.com/w/cpp/chrono/time_point
  [[nodiscard]] std::span<const StopTime> getNextDepartures(std::string const& stopId, std::chrono::system_clock::time_point const& aDateTime, int limit);

};

} // gtfs

#endif //GTFSSCHEDULE_H
