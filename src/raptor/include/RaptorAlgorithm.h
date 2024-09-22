//
// Created by MichaelBrunner on 20/07/2024.
//

#ifndef RAPTOR_ALGORITHM_H
#define RAPTOR_ALGORITHM_H

#include "Connection.h"


#include <vector>
#include <map>
#include <string>
#include <chrono>
#include <memory>
#include <raptor_export.h>

namespace raptor {


  class RAPTOR_API RaptorAlgorithm {
  public:
    // Destructor
    virtual ~RaptorAlgorithm() = default;

    // Method to route the earliest arrival
    [[nodiscard]] virtual std::vector<std::unique_ptr<Connection>> routeEarliestArrival(
      const std::map<std::string, types::raptorInt>& departureStops,
      const std::map<std::string, types::raptorInt>& arrivalStops,
      const config::QueryConfig& config) const
      = 0;

    // Method to route latest departure
    [[nodiscard]] virtual std::vector<std::shared_ptr<Connection>> routeLatestDeparture(
      const std::map<std::string, types::raptorIdx>& departureStops,
      const std::map<std::string, std::chrono::system_clock::time_point>& arrivalStops,
      const config::QueryConfig& config) const
      = 0;

    // Method to route isolines
    [[nodiscard]] virtual std::map<std::string, std::shared_ptr<Connection>> routeIsolines(
      const std::map<std::string, std::chrono::system_clock::time_point>& sourceStops,
      const config::QueryConfig& config) const
      = 0;

  protected:
    RaptorAlgorithm() = default;
  };

}

#endif // RAPTOR_ALGORITHM_H
