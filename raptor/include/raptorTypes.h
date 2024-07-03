//
// Created by MichaelBrunner on 27/06/2024.
//

#ifndef RAPTOR_TYPES_H
#define RAPTOR_TYPES_H

#include <model/Stop.h>
#include <limits>

namespace raptor::utils {
  static constexpr auto INFINITY_VALUE = std::numeric_limits<int>::max();

  struct ConnectionRequest
  {
    // schedule::gtfs::Stop departureStop;
    std::string departureStopId{};
    std::string arrivalStopId{};
    int departureTime{};
    // schedule::gtfs::Stop arrivalStop;
  };
}

#endif //RAPTOR_TYPES_H
