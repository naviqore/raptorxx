//
// Created by MichaelBrunner on 20/07/2024.
//

#include "RaptorRouter.h"

#include "LabelPostprocessor.h"
#include "LoggerFactory.h"
#include "Query.h"
#include "utils/helperFunctions.h"

#include <algorithm>
#include <stdexcept>
#include <iostream>
#include <chrono>
#include <limits>
#include <utility>
#include <vector>
#include <map>
#include <ranges>
#include <sstream>

#include "utils/scopedTimer.h"

namespace raptor {

  namespace validation {
    constexpr types::raptorInt MIN_WALKING_TIME_TO_TARGET = 0;
    constexpr types::raptorInt MAX_DIFFERENCE_IN_SOURCE_STOP_TIMES = 24 * 60 * 60;

    void checkNonNullOrEmptyStops(const std::map<std::string, types::raptorInt>& stops, const std::string& labelSource)
    {
      if (stops.empty()) {
        throw std::invalid_argument(labelSource + " stops must not be empty.");
      }
    }

    void validateSourceStopTimes(const std::map<std::string, types::raptorIdx>& sourceStops)
    {
      for (const auto& entry : sourceStops) {
        if (entry.second == 0) {
          throw std::invalid_argument("Source stop times must not be null.");
        }
      }

      if (const auto minMaxPair = std::minmax_element(sourceStops.begin(), sourceStops.end(), [](const auto& a, const auto& b) { return a.second < b.second; }); minMaxPair.second->second - minMaxPair.first->second > MAX_DIFFERENCE_IN_SOURCE_STOP_TIMES) {
        throw std::invalid_argument("Difference between source stop times must be less than 24 hours.");
      }
    }

    void validateStopPermutations(const std::map<std::string, types::raptorInt>& sourceStops, const std::map<std::string, types::raptorInt>& targetStops)
    {
      // Ensure departure and arrival stops are not the same
      std::set<std::string> sourceKeys;
      for (const auto& key : sourceStops | std::views::keys) {
        sourceKeys.insert(key);
      }

      for (const auto& stop : targetStops | std::views::keys) {
        if (sourceKeys.contains(stop)) {
          throw std::invalid_argument("Source and target stop IDs must not be the same.");
        }
      }
    }


    std::map<types::raptorInt, types::raptorInt> validateStopsAndGetIndices(const std::map<std::string, types::raptorInt>& stops, const std::unordered_map<std::string, types::raptorIdx>& stopsToIdx)
    {
      if (stops.empty()) {
        throw std::invalid_argument("At least one stop ID must be provided.");
      }

      std::map<types::raptorInt, types::raptorInt> validStopIds;
      for (const auto& [stopId, time] : stops) {
        if (auto it = stopsToIdx.find(stopId);
            it != stopsToIdx.end()) {
#if LOGGER
          getConsoleLogger(LoggerName::RAPTOR)->info(std::to_string(it->second));
          getConsoleLogger(LoggerName::RAPTOR)->info(std::to_string(time));
#endif

          validStopIds[it->second] = time;
        }
        else {
#if LOGGER
          getConsoleLogger(LoggerName::RAPTOR)->error(std::format("Stop ID {} not found in lookup, removing from query.", stopId));
#endif
        }
      }

      if (validStopIds.empty()) {
        throw std::invalid_argument("No valid stops provided.");
      }

      return validStopIds;
    }

  }

  RaptorRouter::RaptorRouter(RaptorData raptorData)
    : raptorData(std::move(raptorData))
  {
  }

  std::vector<std::unique_ptr<Connection>> RaptorRouter::routeEarliestArrival(const std::map<std::string, types::raptorInt>& departureStops,
                                                                              const std::map<std::string, types::raptorInt>& arrivalStops,
                                                                              const config::QueryConfig& config) const
  {

    // MEASURE_FUNCTION();

    validation::checkNonNullOrEmptyStops(departureStops, "Departure");
    validation::checkNonNullOrEmptyStops(arrivalStops, "Arrival");

    auto validatedSourceStops = validation::validateStopsAndGetIndices(departureStops, raptorData.getLookup().stops);
    auto validatedTargetStops = validation::validateStopsAndGetIndices(arrivalStops, raptorData.getLookup().stops);

    std::vector<types::raptorIdx> sourceStopIndices;
    std::vector<types::raptorIdx> targetStopIndices;
    std::vector<types::raptorInt> sourceTimes;
    std::vector<types::raptorInt> walkingDurationsToTarget;

    sourceStopIndices.reserve(validatedSourceStops.size());
    targetStopIndices.reserve(validatedTargetStops.size());
    sourceTimes.reserve(validatedSourceStops.size());
    walkingDurationsToTarget.reserve(validatedTargetStops.size());

    for (const auto& [key, value] : validatedSourceStops) {
      sourceStopIndices.push_back(key);
      sourceTimes.push_back(value);
    }

    for (const auto& [key, value] : validatedTargetStops) {
      targetStopIndices.push_back(key);
      walkingDurationsToTarget.push_back(value);
    }

    const auto queryParams = QueryParams{
      .raptorData = raptorData,
      .sourceStopIndices = sourceStopIndices,
      .targetStopIndices = targetStopIndices,
      .sourceTimes = sourceTimes,
      .walkingDurationsToTarget = walkingDurationsToTarget,
      .config = config};

    auto query = Query(queryParams);
    const auto& bestLabelsPerRound = query.run();

    auto referenceDate = *std::ranges::min_element(sourceTimes);

    auto connection = LabelPostprocessor(*this);
    return connection.reconstructParetoOptimalSolutions(bestLabelsPerRound, validatedTargetStops, referenceDate);
  }

  std::vector<std::shared_ptr<Connection>> RaptorRouter::routeLatestDeparture(const std::map<std::string, types::raptorIdx>& departureStops, const std::map<std::string, std::chrono::system_clock::time_point>& arrivalStops, const config::QueryConfig& config) const
  {
    throw std::runtime_error("Not implemented");
  }

  std::map<std::string, std::shared_ptr<Connection>> RaptorRouter::routeIsolines(const std::map<std::string, std::chrono::system_clock::time_point>& sourceStops, const config::QueryConfig& config) const
  {
    throw std::runtime_error("Not implemented");
  }

  const RaptorData& RaptorRouter::getRaptorData() const
  {
    return raptorData;
  }


  std::vector<std::shared_ptr<Connection>> RaptorRouter::getConnections(const std::map<std::string, types::raptorIdx>& sourceStops, const std::map<std::string, types::raptorIdx>& targetStops, const config::QueryConfig& config) const
  {
    throw std::runtime_error("Not implemented");
  }
}
