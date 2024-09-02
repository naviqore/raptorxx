//
// Created by MichaelBrunner on 27/06/2024.
//

#ifndef RAPTOR_TYPES_H
#define RAPTOR_TYPES_H

#include <vector>
#include <limits>
#include <string>

namespace raptor::utils {
  static constexpr auto INFINITY_VALUE = std::numeric_limits<int>::max();

  using stop_id = std::string;
  using route_id = std::string;

  struct ConnectionRequest
  {
    explicit ConnectionRequest(std::vector<std::string>&& departureStopId,
                               std::vector<std::string>&& arrivalStopId,
                               const unsigned int earliestDepartureTime)
      : departureStopId(std::move(departureStopId))
      , arrivalStopId(std::move(arrivalStopId))
      , earliestDepartureTime(earliestDepartureTime) {
      latestDepartureTime = earliestDepartureTime + 3'600;
    }
    std::vector<std::string> departureStopId{};
    std::vector<std::string> arrivalStopId{};
    unsigned int earliestDepartureTime{};
    unsigned int latestDepartureTime{};
  };
}

#endif //RAPTOR_TYPES_H
