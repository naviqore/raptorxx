//
// Created by MichaelBrunner on 20/07/2024.
//

#include "RaptorRouter.h"

#include "LabelPostprocessor.h"
#include "LoggerFactory.h"
#include "Query.h"

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


namespace raptor {

  namespace validation {
    constexpr types::raptorInt MIN_WALKING_TIME_TO_TARGET = 0;
    constexpr types::raptorInt MAX_DIFFERENCE_IN_SOURCE_STOP_TIMES = 24 * 60 * 60;

    void checkNonNullOrEmptyStops(const std::map<std::string, types::raptorInt>& stops, const std::string& labelSource) {
      if (stops.empty())
      {
        throw std::invalid_argument(labelSource + " stops must not be empty.");
      }
    }

    void validateSourceStopTimes(const std::map<std::string, types::raptorIdx>& sourceStops) {
      // Check that no null values are present
      for (const auto& entry : sourceStops)
      {
        if (entry.second == 0)
        { // Assuming '0' is used as a placeholder for null
          throw std::invalid_argument("Source stop times must not be null.");
        }
      }

      // Get min and max values
      auto minMaxPair = std::minmax_element(sourceStops.begin(), sourceStops.end(), [](const auto& a, const auto& b) { return a.second < b.second; });
      if (minMaxPair.second->second - minMaxPair.first->second > MAX_DIFFERENCE_IN_SOURCE_STOP_TIMES)
      {
        throw std::invalid_argument("Difference between source stop times must be less than 24 hours.");
      }
    }

    void validateStopPermutations(const std::map<std::string, types::raptorInt>& sourceStops, const std::map<std::string, types::raptorInt>& targetStops) {
      // Ensure departure and arrival stops are not the same
      std::set<std::string> sourceKeys;
      for (const auto& key : sourceStops | std::views::keys)
      {
        sourceKeys.insert(key);
      }

      for (const auto& stop : targetStops | std::views::keys)
      {
        if (sourceKeys.contains(stop))
        {
          throw std::invalid_argument("Source and target stop IDs must not be the same.");
        }
      }
    }


    std::map<types::raptorInt, types::raptorInt> validateStopsAndGetIndices(const std::map<std::string, types::raptorInt>& stops, const std::unordered_map<std::string, types::raptorIdx>& stopsToIdx) {
      if (stops.empty())
      {
        throw std::invalid_argument("At least one stop ID must be provided.");
      }

      std::map<types::raptorInt, types::raptorInt> validStopIds;
      for (const auto& [stopId, time] : stops)
      {
        if (auto it = stopsToIdx.find(stopId);
            it != stopsToIdx.end())
        {
          getConsoleLogger(LoggerName::RAPTOR)->info(std::to_string(it->second));
          getConsoleLogger(LoggerName::RAPTOR)->info(std::to_string(time));
          validStopIds[it->second] = time;
        }
        else
        {
          std::cerr << "Stop ID " << stopId << " not found in lookup, removing from query." << std::endl;
        }
      }

      if (validStopIds.empty())
      {
        throw std::invalid_argument("No valid stops provided.");
      }

      return validStopIds;
    }

  }

  // Constructor
  RaptorRouter::RaptorRouter(RaptorData raptorData)
    : raptorData(std::move(raptorData))
  // , validator(raptorData.getLookup().stops)
  {}

  std::vector<std::unique_ptr<Connection>> RaptorRouter::routeEarliestArrival(const std::map<std::string, types::raptorInt>& departureStops, const std::map<std::string, types::raptorInt>& arrivalStops, const config::QueryConfig& config) const {
    validation::checkNonNullOrEmptyStops(departureStops, "Departure");
    validation::checkNonNullOrEmptyStops(arrivalStops, "Arrival");

    std::cout << "Routing earliest arrival from ";
    for (const auto& id : departureStops | std::views::keys)
    {
      std::cout << id << " ";
    }
    std::cout << "to ";
    for (const auto& id : arrivalStops | std::views::keys)
    {
      std::cout << id << " ";
    }
    std::cout << "departing at ";
    for (const auto& [id, time] : departureStops)
    {
      std::cout << time << " ";
    }
    std::cout << std::endl;

    auto validatedSourceStops = validation::validateStopsAndGetIndices(departureStops, raptorData.getLookup().stops);
    auto validatedTargetStops = validation::validateStopsAndGetIndices(arrivalStops, raptorData.getLookup().stops);

    const auto sourceStopIndices = validatedSourceStops | std::views::keys | std::ranges::to<std::vector<types::raptorIdx>>();
    const auto targetStopIndices = validatedTargetStops | std::views::keys | std::ranges::to<std::vector<types::raptorIdx>>();
    const auto sourceTimes = validatedSourceStops | std::views::values | std::ranges::to<std::vector<types::raptorInt>>();
    const auto walkingDurationsToTarget = validatedTargetStops | std::views::values | std::ranges::to<std::vector<types::raptorInt>>();

    const auto queryParams = QueryParams{
      .raptorData = raptorData,
      .sourceStopIndices = sourceStopIndices,
      .targetStopIndices = targetStopIndices,
      .sourceTimes = sourceTimes,
      .walkingDurationsToTarget = walkingDurationsToTarget,
      .config = config};

    auto query = Query(queryParams);
    const auto& bestLabelsPerRound = query.run();

    auto connection = LabelPostprocessor(*this);

    auto referenceDate = std::ranges::min(validatedSourceStops | std::views::values);

    // validatedSourceStops = std::map<types::raptorInt, types::raptorInt>{{1, 0}};
    return connection.reconstructParetoOptimalSolutions(bestLabelsPerRound, validatedTargetStops, referenceDate);
  }

  std::vector<std::shared_ptr<Connection>> RaptorRouter::routeLatestDeparture(const std::map<std::string, types::raptorIdx>& departureStops, const std::map<std::string, std::chrono::system_clock::time_point>& arrivalStops, const config::QueryConfig& config) const {
    throw std::runtime_error("Not implemented");
  }

  std::map<std::string, std::shared_ptr<Connection>> RaptorRouter::routeIsolines(const std::map<std::string, std::chrono::system_clock::time_point>& sourceStops, const config::QueryConfig& config) const {
    throw std::runtime_error("Not implemented");
  }

  const RaptorData& RaptorRouter::getRaptorData() const {
    return raptorData;
  }


  std::vector<std::shared_ptr<Connection>> RaptorRouter::getConnections(const std::map<std::string, types::raptorIdx>& sourceStops, const std::map<std::string, types::raptorIdx>& targetStops, const config::QueryConfig& config) const {
    validation::validateSourceStopTimes(sourceStops);

    // Mocked implementation for processing
    std::vector<std::shared_ptr<Connection>> connections;
    // Implement actual logic using DateTimeUtils, Query, LabelPostprocessor, etc.
    return connections;
  }


  types::raptorInt secondsOfDay(const std::chrono::system_clock::time_point& timePoint) {
    const std::time_t time = std::chrono::system_clock::to_time_t(timePoint);
    std::tm localTime{};
    // Use localtime_s for thread safety on Windows
    if (localtime_s(&localTime, &time) != 0)
    {
      throw std::runtime_error("Unable to convert time_point to local time.");
    }

    return localTime.tm_hour * 3600 + localTime.tm_min * 60 + localTime.tm_sec;
  }
}