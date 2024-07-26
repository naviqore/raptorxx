//
// Created by MichaelBrunner on 20/07/2024.
//

#include "RaptorRouter.h"

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

  // Constructor
  RaptorRouter::RaptorRouter(const RaptorData& raptorData)
    : raptorData(raptorData)
    , validator(raptorData.getLookup().stops) {}

  std::vector<Connection> RaptorRouter::routeEarliestArrival(const std::map<std::string, int>& departureStops, const std::map<std::string, int>& arrivalStops, const config::QueryConfig& config) const {
    validator.checkNonNullOrEmptyStops(departureStops, "Departure");
    validator.checkNonNullOrEmptyStops(arrivalStops, "Arrival");

    std::cout << "Routing earliest arrival from ";
    for (const auto& [id, time] : departureStops)
    {
      std::cout << id << " ";
    }
    std::cout << "to ";
    for (const auto& [id, time] : arrivalStops)
    {
      std::cout << id << " ";
    }
    std::cout << "departing at ";
    for (const auto& [id, time] : departureStops)
    {
      std::cout << time << " ";
    }
    std::cout << std::endl;

    auto validatedSourceStops = validator.validateStopsAndGetIndices(departureStops);
    auto validatedTargetStops = validator.validateStopsAndGetIndices(arrivalStops);
    InputValidator::validateStopPermutations(departureStops, arrivalStops);

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
    auto labels = query.run();

    return {};
  }
  std::vector<Connection> RaptorRouter::routeLatestDeparture(const std::map<std::string, int>& departureStops, const std::map<std::string, std::chrono::system_clock::time_point>& arrivalStops, const config::QueryConfig& config) const {
    throw std::runtime_error("Not implemented");
  }
  std::map<std::string, Connection> RaptorRouter::routeIsolines(const std::map<std::string, std::chrono::system_clock::time_point>& sourceStops, const config::QueryConfig& config) const {
    throw std::runtime_error("Not implemented");
  }


  std::vector<Connection> RaptorRouter::getConnections(const std::map<std::string, int>& sourceStops, const std::map<std::string, int>& targetStops, const config::QueryConfig& config) const {
    validator.validateSourceStopTimes(sourceStops);

    // Mocked implementation for processing
    std::vector<Connection> connections;
    // Implement actual logic using DateTimeUtils, Query, LabelPostprocessor, etc.
    return connections;
  }
  // InputValidator implementation
  RaptorRouter::InputValidator::InputValidator(const std::unordered_map<std::string, int>& stopsToIdx)
    : stopsToIdx(stopsToIdx) {}

  void RaptorRouter::InputValidator::checkNonNullOrEmptyStops(
    const std::map<std::string, int>& stops, const std::string& labelSource) const {
    if (stops.empty())
    {
      throw std::invalid_argument(labelSource + " stops must not be empty.");
    }
  }

  void RaptorRouter::InputValidator::validateSourceStopTimes(
    const std::map<std::string, int>& sourceStops) const {
    // Implementation to validate stop times
    // Example: Check for null values and time difference
  }
  void RaptorRouter::InputValidator::validateStopPermutations(const std::map<std::string, int>& sourceStops, const std::map<std::string, int>& targetStops) {
  }



  std::map<int, int> RaptorRouter::InputValidator::validateStopsAndGetIndices(
    const std::map<std::string, int>& stops) const {
    std::map<int, int> validStopIds;
    for (const auto& [stopId, time] : stops)
    {
      auto it = stopsToIdx.find(stopId);
      if (it != stopsToIdx.end())
      {
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


  int secondsOfDay(const std::chrono::system_clock::time_point& timePoint) {
    std::time_t time = std::chrono::system_clock::to_time_t(timePoint);
    std::tm localTime{};
    // Use localtime_s for thread safety on Windows
    if (localtime_s(&localTime, &time) != 0)
    {
      throw std::runtime_error("Unable to convert time_point to local time.");
    }

    return localTime.tm_hour * 3600 + localTime.tm_min * 60 + localTime.tm_sec;
  }
}