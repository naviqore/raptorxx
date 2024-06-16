//
// Created by MichaelBrunner on 16/06/2024.
//

#include "GtfsSchedule.h"
#include <algorithm>
#include <ranges>

namespace gtfs {

  GtfsSchedule::GtfsSchedule(const GtfsData& data)
    : relationManager(data) {}

  std::span<const StopTime> GtfsSchedule::getNextDepartures(std::string const& stopId, std::chrono::system_clock::time_point const& aDateTime, int limit) {
    /*auto data = relationManager.getData().stopTimes
                | std::views::filter([stopId, aDateTime](const StopTime& stopTime) {
                    return stopTime.departureTime
                  });*/

    // TODO in next Jira ticket
    std::ignore = stopId;
    std::ignore = aDateTime;
    std::ignore = limit;

    return {};
  }
} // gtfs