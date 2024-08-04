//
// Created by MichaelBrunner on 20/07/2024.
//

#include "Query.h"

#include "FootpathRelaxer.h"
#include "LoggerFactory.h"
#include "RouteScanner.h"

#include <stdexcept>
#include <iostream>
#include <limits>


namespace raptor {


  Query::Query(const QueryParams& params)
    : sourceStopIndices(params.sourceStopIndices)
    , targetStopIndices(params.targetStopIndices)
    , sourceTimes(params.sourceTimes)
    , walkingDurationsToTarget(params.walkingDurationsToTarget)
    , raptorData(params.raptorData)
    , config(params.config)
    , stopLabelsAndTimes(static_cast<int>(this->raptorData.getStopContext().stops.size())) {

    if (sourceStopIndices.size() != sourceTimes.size())
    {
      throw std::invalid_argument("Source stops and departure/arrival times must have the same size.");
    }

    if (targetStopIndices.size() != walkingDurationsToTarget.size())
    {
      throw std::invalid_argument("Target stops and walking durations to target must have the same size.");
    }

    targetStops.resize(targetStopIndices.size() * 2);
    cutoffTime = determineCutoffTime();
  }

  const std::vector<std::vector<std::unique_ptr<StopLabelsAndTimes::Label>>>& Query::run() {

    const auto footpathRelaxer = FootpathRelaxer(stopLabelsAndTimes, raptorData, config.getMinimumTransferDuration(), config.getMaximumWalkingDuration());
    auto routeScanner = RouteScanner(stopLabelsAndTimes, raptorData, config.getMinimumTransferDuration());

    auto markedStops = initialize();
    auto newStops = footpathRelaxer.relaxInitial(sourceStopIndices);
    markedStops.insert(newStops.begin(), newStops.end());
    markedStops = removeSuboptimalLabelsForRound(0, markedStops);

    types::raptorInt round = 1;
    while (!markedStops.empty() && (round - 1) <= config.getMaximumTransferNumber())
    {
      stopLabelsAndTimes.addNewRound();

      auto markedStopsNext = routeScanner.scan(round, markedStops);
      auto relaxedStops = footpathRelaxer.relax(round, markedStopsNext);
      markedStopsNext.insert(relaxedStops.begin(), relaxedStops.end());

      markedStops = removeSuboptimalLabelsForRound(round, markedStopsNext);
      round++;
    }

    return stopLabelsAndTimes.getBestLabelsPerRound();
  }

  std::unordered_set<types::raptorIdx> Query::initialize() {
    getConsoleLogger(LoggerName::RAPTOR)->info("Initializing global best times per stop and best labels per round");

    for (size_t i = 0; i < targetStopIndices.size(); ++i)
    {
      targetStops[i * 2] = targetStopIndices[i];
      targetStops[i * 2 + 1] = walkingDurationsToTarget[i];
    }

    std::unordered_set<types::raptorIdx> markedStops{};
    for (size_t i = 0; i < sourceStopIndices.size(); ++i)
    {
      auto currentStopIdx = sourceStopIndices[i];
      auto targetTime = sourceTimes[i];

      auto label = std::make_unique<StopLabelsAndTimes::Label>(0, targetTime, StopLabelsAndTimes::LabelType::INITIAL, types::NO_INDEX, types::NO_INDEX, currentStopIdx, nullptr);
      stopLabelsAndTimes.setLabel(0, currentStopIdx, std::move(label));
      stopLabelsAndTimes.setBestTime(currentStopIdx, targetTime);

      markedStops.insert(currentStopIdx);
    }

    return markedStops;
  }

  std::unordered_set<types::raptorIdx> Query::removeSuboptimalLabelsForRound(types::raptorInt round, const std::unordered_set<types::raptorIdx>& markedStops) {
    const auto bestTime = getBestTimeForAllTargetStops();
    if (bestTime == types::INFINITY_VALUE_MAX || bestTime == types::INFINITY_VALUE_MIN)
    {
      return markedStops;
    }

    std::unordered_set<types::raptorIdx> markedStopsClean;
    for (const auto stopIdx : markedStops)
    {
      if (const auto label = stopLabelsAndTimes.getLabel(round, stopIdx))
      {
        if ((label->targetTime > bestTime) || (label->targetTime < bestTime))
        {
          stopLabelsAndTimes.setLabel(round, stopIdx, nullptr);
        }
        else
        {
          markedStopsClean.insert(stopIdx);
        }
      }
    }

    return markedStopsClean;
  }

  types::raptorIdx Query::getBestTimeForAllTargetStops() const {
    auto bestTime = cutoffTime;

    for (auto i = 0; i < targetStops.size(); i += 2)
    {
      const auto targetStopIdx = targetStops[i];
      const auto walkDurationToTarget = targetStops[i + 1];

      if (auto bestTimeForStop = stopLabelsAndTimes.getActualBestTime(targetStopIdx);
          bestTimeForStop != std::numeric_limits<int>::max())
      {
        bestTimeForStop += walkDurationToTarget;
        bestTime = std::min(bestTime, bestTimeForStop);
      }
    }

    return bestTime;
  }

  types::raptorIdx Query::determineCutoffTime() const {
    if (config.getMaximumTravelTime() == types::INFINITY_VALUE_MAX)
    {
      return types::INFINITY_VALUE_MAX;
    }
    const auto latestArrival = *std::ranges::max_element(sourceTimes);
    return latestArrival - config.getMaximumTravelTime();
  }

} // raptor