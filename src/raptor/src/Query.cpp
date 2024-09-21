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
#include <utility>

#include "utils/scopedTimer.h"


namespace raptor {


  Query::Query(QueryParams params)
    : sourceStopIndices(std::move(params.sourceStopIndices))
    , targetStopIndices(std::move(params.targetStopIndices))
    , sourceTimes(std::move(params.sourceTimes))
    , walkingDurationsToTarget(std::move(params.walkingDurationsToTarget))
    , raptorData(params.raptorData)
    , config(params.config)
    , stopLabelsAndTimes(static_cast<int>(this->raptorData.getStopContext().stops.size()))
  {

    if (sourceStopIndices.size() != sourceTimes.size()) {
      throw std::invalid_argument("Source stops and departure/arrival times must have the same size.");
    }

    if (targetStopIndices.size() != walkingDurationsToTarget.size()) {
      throw std::invalid_argument("Target stops and walking durations to target must have the same size.");
    }

    targetStops.resize(targetStopIndices.size() * 2);
    cutoffTime = determineCutoffTime();
  }

  const std::vector<std::vector<std::unique_ptr<StopLabelsAndTimes::Label>>>& Query::run()
  {
    MEASURE_FUNCTION();
    const auto footpathRelaxer = FootpathRelaxer(stopLabelsAndTimes, raptorData, config.getMinimumTransferDuration(), config.getMaximumWalkingDuration());
    auto routeScanner = RouteScanner(stopLabelsAndTimes, raptorData, config.getMinimumTransferDuration());

    initialize();
    footpathRelaxer.relaxInitial();
    removeSuboptimalLabelsForRound(0);

    while (stopLabelsAndTimes.hasMarkedStops() && static_cast<unsigned int>(stopLabelsAndTimes.getRound()) < config.getMaximumTransferNumber()) {
      stopLabelsAndTimes.addNewRound();
      routeScanner.scan(stopLabelsAndTimes.getRound());
      footpathRelaxer.relax(stopLabelsAndTimes.getRound());
      removeSuboptimalLabelsForRound(stopLabelsAndTimes.getRound());
    }

    return stopLabelsAndTimes.getBestLabelsPerRound();
  }

  void Query::initialize()
  {
    // MEASURE_FUNCTION();
#if LOGGER
    getConsoleLogger(LoggerName::RAPTOR)->info("Initializing global best times per stop and best labels per round");
#endif


    for (size_t i = 0; i < targetStopIndices.size(); ++i) {
      targetStops[i * 2] = targetStopIndices[i];
      targetStops[i * 2 + 1] = walkingDurationsToTarget[i];
    }

    for (size_t i = 0; i < sourceStopIndices.size(); ++i) {
      auto currentStopIdx = sourceStopIndices[i];
      auto targetTime = sourceTimes[i];

      auto label = std::make_unique<StopLabelsAndTimes::Label>(0, targetTime, StopLabelsAndTimes::LabelType::INITIAL, types::NO_INDEX, types::NO_INDEX, currentStopIdx, nullptr);
      stopLabelsAndTimes.setLabel(0, currentStopIdx, std::move(label));
      stopLabelsAndTimes.setBestTime(currentStopIdx, targetTime);
      stopLabelsAndTimes.mark(currentStopIdx);
    }
  }

  void Query::removeSuboptimalLabelsForRound(const int round)
  {
    // MEASURE_FUNCTION();
    const auto bestTime = getBestTimeForAllTargetStops();
    if (bestTime == types::INFINITY_VALUE_MAX) {

      return;
    }

    for (auto stopIndex{0}; stopIndex < raptorData.getStopContext().stops.size(); stopIndex++) {

      if (false == stopLabelsAndTimes.isMarkedNextRound(stopIndex)) {
        continue;
      }

      if (const auto label = stopLabelsAndTimes.getLabel(round, stopIndex);
          label != nullptr) {

        if (label->targetTime > bestTime) {
          stopLabelsAndTimes.setLabel(round, stopIndex, nullptr);
          stopLabelsAndTimes.unmark(stopIndex);
        }
      }
    }
  }

  types::raptorIdx Query::getBestTimeForAllTargetStops() const
  {
    auto bestTime = cutoffTime;

    for (auto i = 0; i < targetStops.size(); i += 2) {
      const auto targetStopIdx = targetStops[i];
      const auto walkDurationToTarget = targetStops[i + 1];

      if (auto bestTimeForStop = stopLabelsAndTimes.getActualBestTime(targetStopIdx);
          bestTimeForStop != types::INFINITY_VALUE_MAX) {
        bestTimeForStop += walkDurationToTarget;
        bestTime = std::min(bestTime, bestTimeForStop);
      }
    }

    return bestTime;
  }

  types::raptorIdx Query::determineCutoffTime() const
  {
    if (config.getMaximumTravelTime() == types::INFINITY_VALUE_MAX) {
      return types::INFINITY_VALUE_MAX;
    }
    const auto latestArrival = *std::ranges::max_element(sourceTimes);
    return latestArrival - config.getMaximumTravelTime();
  }

} // raptor
