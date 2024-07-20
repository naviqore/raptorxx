//
// Created by MichaelBrunner on 20/07/2024.
//

#ifndef RAPTOR_ALGORITHM_H
#define RAPTOR_ALGORITHM_H

#include <vector>
#include <map>
#include <string>
#include <chrono>
#include <memory>
#include <raptor_export.h>

namespace raptor {
  // Forward declarations
  class Connection;

  // Define RaptorAlgorithm interface
  class RAPTOR_API RaptorAlgorithm
  {
  public:
    // Destructor
    virtual ~RaptorAlgorithm() = default;

    // Static builder method
    static std::unique_ptr<RaptorAlgorithm> create(int sameStopTransferTime);

    // Method to route earliest arrival
    virtual std::vector<Connection> routeEarliestArrival(
      const std::map<std::string, int>& departureStops, // std::chrono::system_clock::time_point
      const std::map<std::string, int>& arrivalStops,
      const QueryConfig& config) const
      = 0;

    // Method to route latest departure
    virtual std::vector<Connection> routeLatestDeparture(
      const std::map<std::string, int>& departureStops,
      const std::map<std::string, std::chrono::system_clock::time_point>& arrivalStops,
      const QueryConfig& config) const
      = 0;

    // Method to route isolines
    virtual std::map<std::string, Connection> routeIsolines(
      const std::map<std::string, std::chrono::system_clock::time_point>& sourceStops,
      const QueryConfig& config) const
      = 0;

  protected:
    // Protected constructor to prevent direct instantiation
    RaptorAlgorithm() = default;
  };

}

#endif // RAPTOR_ALGORITHM_H
