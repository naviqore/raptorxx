//
// Created by MichaelBrunner on 20/07/2024.
//

#include "Query.h"

#include "FootpathRelaxer.h"
#include "RouteScanner.h"

#include <stdexcept>
#include <iostream>
#include <limits>


namespace raptor {

  Query::Query(const RaptorData& raptorData, const std::vector<int>& sourceStopIndices, const std::vector<int>& targetStopIndices, const std::vector<int>& sourceTimes, const std::vector<int>& walkingDurationsToTarget, const QueryConfig& config)
    : raptorData(raptorData)
    , sourceStopIndices(sourceStopIndices)
    , targetStopIndices(targetStopIndices)
    , sourceTimes(sourceTimes)
    , walkingDurationsToTarget(walkingDurationsToTarget)
    , config(config)
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

  std::vector<std::vector<std::shared_ptr<StopLabelsAndTimes::Label>>> Query::run() {

    FootpathRelaxer footpathRelaxer(stopLabelsAndTimes, raptorData, config.getMinimumTransferDuration(), config.getMaximumWalkingDuration());

    RouteScanner routeScanner(stopLabelsAndTimes, raptorData, config.getMinimumTransferDuration());

    std::unordered_set<int> markedStops = initialize();
    auto newStops = footpathRelaxer.relaxInitial(sourceStopIndices);
    markedStops.insert(newStops.begin(), newStops.end());
    markedStops = removeSuboptimalLabelsForRound(0, markedStops);

    int round = 1;
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

  std::unordered_set<int> Query::initialize() {
    std::cout << "Initializing global best times per stop and best labels per round" << std::endl;

    for (size_t i = 0; i < targetStopIndices.size(); ++i)
    {
      targetStops[i * 2] = targetStopIndices[i];
      targetStops[i * 2 + 1] = walkingDurationsToTarget[i];
    }

    std::unordered_set<int> markedStops;
    for (size_t i = 0; i < sourceStopIndices.size(); ++i)
    {
      int currentStopIdx = sourceStopIndices[i];
      int targetTime = sourceTimes[i];

      auto label = std::make_shared<StopLabelsAndTimes::Label>(0, targetTime, StopLabelsAndTimes::LabelType::INITIAL, StopLabelsAndTimes::NO_INDEX, StopLabelsAndTimes::NO_INDEX, currentStopIdx, nullptr);
      stopLabelsAndTimes.setLabel(0, currentStopIdx, label);
      stopLabelsAndTimes.setBestTime(currentStopIdx, targetTime);

      markedStops.insert(currentStopIdx);
    }

    return markedStops;
  }

  std::unordered_set<int> Query::removeSuboptimalLabelsForRound(int round, const std::unordered_set<int>& markedStops) {
    int bestTime = getBestTimeForAllTargetStops();
    if (bestTime == std::numeric_limits<int>::max() || bestTime == std::numeric_limits<int>::min())
    {
      return markedStops;
    }

    std::unordered_set<int> markedStopsClean;
    for (int stopIdx : markedStops)
    {
      auto label = stopLabelsAndTimes.getLabel(round, stopIdx);
      if (label)
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

  int Query::getBestTimeForAllTargetStops() const {
    int bestTime = cutoffTime;

    for (size_t i = 0; i < targetStops.size(); i += 2)
    {
      int targetStopIdx = targetStops[i];
      int walkDurationToTarget = targetStops[i + 1];
      int bestTimeForStop = stopLabelsAndTimes.getActualBestTime(targetStopIdx);

      if (bestTimeForStop != std::numeric_limits<int>::max())
      {
        bestTimeForStop += walkDurationToTarget;
        bestTime = std::min(bestTime, bestTimeForStop);
      }
    }

    return bestTime;
  }

  int Query::determineCutoffTime() const {
    if (config.getMaximumTravelTime() == std::numeric_limits<int>::max())
    {
      return  std::numeric_limits<int>::max();
    }
    else
    {
      int latestArrival = *std::max_element(sourceTimes.begin(), sourceTimes.end());
      return latestArrival - config.getMaximumTravelTime();
    }
  }

} // raptor