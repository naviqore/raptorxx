//
// Created by MichaelBrunner on 11/07/2024.
//

#ifndef LABEL_H
#define LABEL_H

#include "raptorTypes.h"


#include <string>
#include <optional>

namespace raptor::utils {


  struct LabelEarliestArrival
  {
    unsigned int arrivalTime = INFINITY_VALUE;
    unsigned int parentDepartureTime = INFINITY_VALUE;
    stop_id stopId{};
    stop_id parentStopId{};
    route_id routeId{};
    bool useRoute{false};
  };


  struct Label
  {
    unsigned int arrivalTime{};
    int transfers{};
    std::optional<std::string> previousStopId{};
    std::optional<std::string> routeId{};
    std::optional<std::string> tripId{};

    bool operator<(const Label& other) const {
      return std::tie(arrivalTime, transfers) < std::tie(other.arrivalTime, other.transfers);
    }

    bool operator>(const Label& other) const {
      return std::tie(arrivalTime, transfers) > std::tie(other.arrivalTime, other.transfers);
    }

    [[nodiscard]] bool isDominatedBy(const Label& other) const {
      return other.arrivalTime <= arrivalTime && other.transfers <= transfers;
    }
  };

}

#endif //LABEL_H
