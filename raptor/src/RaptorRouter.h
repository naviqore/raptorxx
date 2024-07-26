//
// Created by MichaelBrunner on 20/07/2024.
//

#ifndef RAPTOR_ROUTER_H
#define RAPTOR_ROUTER_H

#include "../include/config/QueryConfig.h"
#include "RaptorAlgorithm.h"
#include "utils/RaptorData.h"

#include <map>
#include <vector>
#include <string>
#include <stdexcept>
#include <iostream>
#include <chrono>
#include <raptor_export.h>

int secondsOfDay(const std::chrono::system_clock::time_point& timePoint);

namespace raptor {

  class Connection
  {
  };

  class RAPTOR_API RaptorRouter final : public RaptorAlgorithm
  {
  public:
    // Constructor
    explicit RaptorRouter(const RaptorData& raptorData);

    [[nodiscard]] std::vector<Connection> routeEarliestArrival(const std::map<std::string, int>& departureStops, const std::map<std::string, int>& arrivalStops, const config::QueryConfig& config) const override;

    [[nodiscard]] std::vector<Connection> routeLatestDeparture(const std::map<std::string, int>& departureStops, const std::map<std::string, std::chrono::system_clock::time_point>& arrivalStops, const config::QueryConfig& config) const override;

    [[nodiscard]] std::map<std::string, Connection> routeIsolines(const std::map<std::string, std::chrono::system_clock::time_point>& sourceStops, const config::QueryConfig& config) const override;



  private:
    // Method to get connections
    std::vector<Connection> getConnections(const std::map<std::string, int>& sourceStops,
                                           const std::map<std::string, int>& targetStops,
                                           const config::QueryConfig& config) const;


    // Private nested class for input validation
    class InputValidator
    {
    public:
      explicit InputValidator(const std::unordered_map<std::string, int>& stopsToIdx);
      void checkNonNullOrEmptyStops(const std::map<std::string, int>& stops, const std::string& labelSource) const;
      void validateSourceStopTimes(const std::map<std::string, int>& sourceStops) const;
      static void validateStopPermutations(const std::map<std::string, int>& sourceStops, const std::map<std::string, int>& targetStops);
      [[nodiscard]] std::map<int, int> validateStopsAndGetIndices(const std::map<std::string, int>& stops) const;

    private:
      static constexpr int MIN_WALKING_TIME_TO_TARGET = 0;
      static constexpr int MAX_DIFFERENCE_IN_SOURCE_STOP_TIMES = 24 * 60 * 60;

      const std::unordered_map<std::string, int>& stopsToIdx;
    };

    // Member variables
    RaptorData raptorData;
    InputValidator validator;
  };
}

#endif // RAPTOR_ROUTER_H
