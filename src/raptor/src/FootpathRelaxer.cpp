//
// Created by MichaelBrunner on 20/07/2024.
//

#include "FootpathRelaxer.h"

#include "LoggerFactory.h"
#include "usingTypes.h"

#include <algorithm>
#include <iostream>
#include <format>

namespace raptor {

  FootpathRelaxer::FootpathRelaxer(StopLabelsAndTimes& stopLabelsAndTimes, const RaptorData& raptorData, const types::raptorInt minimumTransferDuration, const types::raptorInt maximumWalkingDuration)
    : transfers(raptorData.getStopContext().transfers)
    , stops(raptorData.getStopContext().stops)
    , minTransferDuration(minimumTransferDuration)
    , maxWalkingDuration(maximumWalkingDuration)
    , stopLabelsAndTimes(stopLabelsAndTimes)
  {
  }

  std::unordered_set<types::raptorIdx> FootpathRelaxer::relaxInitial(const std::vector<types::raptorIdx>& stopIndices) const
  {

    std::unordered_set<types::raptorIdx> newlyMarkedStops;

    getConsoleLogger(LoggerName::RAPTOR)->info("Initial relaxing of footpaths for source stops");

    for (const auto sourceStopIdx : stopIndices) {
      expandFootpathsFromStop(sourceStopIdx, 0, newlyMarkedStops);
    }

    return newlyMarkedStops;
  }

  std::unordered_set<types::raptorIdx> FootpathRelaxer::relax(const types::raptorInt round, const std::unordered_set<types::raptorIdx>& stopIndices) const
  {
    std::unordered_set<types::raptorIdx> newlyMarkedStops;
    getConsoleLogger(LoggerName::RAPTOR)->info(std::format("Relaxing footpaths for round {}", round));

    for (const auto sourceStopIdx : stopIndices) {
      expandFootpathsFromStop(sourceStopIdx, round, newlyMarkedStops);
    }

    return newlyMarkedStops;
  }

  void FootpathRelaxer::expandFootpathsFromStop(const types::raptorIdx stopIdx, const types::raptorInt round, std::unordered_set<types::raptorInt>& markedStops) const
  {
    if (stops[stopIdx].numberOfTransfers == 0) {
      return;
    }

    const auto& stop = stops[stopIdx];
    const auto previousLabel = stopLabelsAndTimes.getLabel(round, stopIdx);

    if (previousLabel == nullptr || previousLabel->type == StopLabelsAndTimes::LabelType::TRANSFER) {
      return;
    }

    auto sourceTime = previousLabel->targetTime;

    for (auto i = stop.transferIndex; i < stop.transferIndex + static_cast<int>(stop.numberOfTransfers); ++i) {
      const auto& transfer = transfers[i];
      const auto& targetStop = stops[transfer.targetStopIndex];

      if (maxWalkingDuration < transfer.duration) {
        continue;
      }

      auto targetTime = sourceTime + transfer.duration + minTransferDuration;
      const auto comparableTargetTime = targetTime - targetStop.sameStopTransferTime;

      // if label is not improved, continue
      const auto bestTimeToCompareTo = stopLabelsAndTimes.getComparableBestTime(transfer.targetStopIndex);
      if (comparableTargetTime >= bestTimeToCompareTo) {
        continue;
      }
      getConsoleLogger(LoggerName::RAPTOR)->info(std::format("Stop {} was improved by transfer from stop {}", stop.id, targetStop.id));

      stopLabelsAndTimes.setBestTime(transfer.targetStopIndex, comparableTargetTime);

      stopLabelsAndTimes.setLabel(round,
                                  transfer.targetStopIndex,
                                  // directly creating label instead of copying previous label
                                  std::make_unique<StopLabelsAndTimes::Label>(
                                    sourceTime,
                                    targetTime,
                                    StopLabelsAndTimes::LabelType::TRANSFER,
                                    i,
                                    types::NO_INDEX,
                                    transfer.targetStopIndex,
                                    stopLabelsAndTimes.getLabel(round, stopIdx)));

      markedStops.insert(transfer.targetStopIndex);
    }
  }
} // raptor