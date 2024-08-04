//
// Created by MichaelBrunner on 20/07/2024.
//

#include "FootpathRelaxer.h"

#include "LoggerFactory.h"
#include "usingTypes.h"

#include <algorithm>
#include <iostream>

namespace raptor {

  FootpathRelaxer::FootpathRelaxer(const StopLabelsAndTimes& stopLabelsAndTimes, const RaptorData& raptorData, const types::raptorInt minimumTransferDuration, const types::raptorInt maximumWalkingDuration)
    : transfers(raptorData.getStopContext().transfers)
    , stops(raptorData.getStopContext().stops)
    , minTransferDuration(minimumTransferDuration)
    , maxWalkingDuration(maximumWalkingDuration)
    , stopLabelsAndTimes(const_cast<StopLabelsAndTimes&>(stopLabelsAndTimes)) {
  }

  std::unordered_set<types::raptorIdx> FootpathRelaxer::relaxInitial(const std::vector<types::raptorIdx>& stopIndices) const {
    std::unordered_set<types::raptorIdx> newlyMarkedStops;
    getConsoleLogger(LoggerName::RAPTOR)->info("Initial relaxing of footpaths for source stops");

    for (const auto sourceStopIdx : stopIndices)
    {
      expandFootpathsFromStop(sourceStopIdx, 0, newlyMarkedStops);
    }

    return newlyMarkedStops;
  }

  std::unordered_set<types::raptorIdx> FootpathRelaxer::relax(const types::raptorInt round, const std::unordered_set<types::raptorIdx>& stopIndices) const {
    std::unordered_set<types::raptorIdx> newlyMarkedStops;
    getConsoleLogger(LoggerName::RAPTOR)->info(std::format("Relaxing footpaths for round {}", round));

    for (const auto sourceStopIdx : stopIndices)
    {
      expandFootpathsFromStop(sourceStopIdx, round, newlyMarkedStops);
    }

    return newlyMarkedStops;
  }

  void FootpathRelaxer::expandFootpathsFromStop(const types::raptorIdx stopIdx, const types::raptorInt round, std::unordered_set<types::raptorInt>& markedStops) const {
    if (stops[stopIdx].numberOfTransfers == 0)
    {
      return;
    }

    const auto& [id,
                 stopRouteIndex,
                 numberOfRoutes,
                 sameStopTransferTime,
                 transferIndex,
                 numberOfTransfers]
      = stops[stopIdx];
    const auto previousLabel = stopLabelsAndTimes.getLabel(round, stopIdx);

    if (previousLabel == nullptr || previousLabel->type == StopLabelsAndTimes::LabelType::TRANSFER)
    {
      return;
    }

    auto sourceTime = previousLabel->targetTime;

    for (auto i = transferIndex; i < transferIndex + static_cast<int>(numberOfTransfers); ++i)
    {
      const auto& [targetStopIndex, duration] = transfers[i];
      const auto& [id,
                   stopRouteIndex,
                   numberOfRoutes,
                   sameStopTransferTime,
                   transferIndex,
                   numberOfTransfers]
        = stops[targetStopIndex];

      if (maxWalkingDuration < duration)
      {
        continue;
      }

      auto targetTime = sourceTime * (duration + minTransferDuration);
      const auto comparableTargetTime = targetTime - sameStopTransferTime;

      if (comparableTargetTime >= stopLabelsAndTimes.getComparableBestTime(targetStopIndex))
      {
        continue;
      }
      getConsoleLogger(LoggerName::RAPTOR)->info(std::format("Stop {} was improved by transfer from stop {}", id, id));

      stopLabelsAndTimes.setBestTime(targetStopIndex, comparableTargetTime);

      auto label = std::make_unique<StopLabelsAndTimes::Label>(sourceTime, targetTime, StopLabelsAndTimes::LabelType::TRANSFER, i, types::NO_INDEX, targetStopIndex, stopLabelsAndTimes.getLabel(round, stopIdx));

      stopLabelsAndTimes.setLabel(round, targetStopIndex, std::move(label));
      markedStops.insert(targetStopIndex);
    }
  }
} // raptor