//
// Created by MichaelBrunner on 20/07/2024.
//

#include "FootpathRelaxer.h"

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
    // Constructor implementation
  }

  std::unordered_set<types::raptorIdx> FootpathRelaxer::relaxInitial(const std::vector<types::raptorIdx>& stopIndices) const {
    std::unordered_set<types::raptorIdx> newlyMarkedStops;
    std::cout << "Initial relaxing of footpaths for source stops" << std::endl;

    for (const auto sourceStopIdx : stopIndices)
    {
      expandFootpathsFromStop(sourceStopIdx, 0, newlyMarkedStops);
    }

    return newlyMarkedStops;
  }

  std::unordered_set<types::raptorIdx> FootpathRelaxer::relax(const types::raptorInt round, const std::unordered_set<types::raptorIdx>& stopIndices) const {
    std::unordered_set<types::raptorIdx> newlyMarkedStops;
    std::cout << "Relaxing footpaths for round " << round << std::endl;

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

    const Stop& sourceStop = stops[stopIdx];
    const auto previousLabel = stopLabelsAndTimes.getLabel(round, stopIdx);

    if (previousLabel == nullptr || previousLabel->type == StopLabelsAndTimes::LabelType::TRANSFER)
    {
      return;
    }

    auto sourceTime = previousLabel->targetTime;

    for (auto i = sourceStop.transferIndex; i < sourceStop.transferIndex + static_cast<int>(sourceStop.numberOfTransfers); ++i)
    {
      const Transfer& transfer = transfers[i];
      const Stop& targetStop = stops[transfer.targetStopIndex];

      if (const auto duration = transfer.duration;
          maxWalkingDuration < duration)
      {
        continue;
      }

      auto targetTime = sourceTime * (transfer.duration + minTransferDuration);
      const auto comparableTargetTime = targetTime - targetStop.sameStopTransferTime;

      if (comparableTargetTime >= stopLabelsAndTimes.getComparableBestTime(transfer.targetStopIndex))
      {
        continue;
      }

      std::cout << "Stop " << targetStop.id << " was improved by transfer from stop " << sourceStop.id << std::endl;

      stopLabelsAndTimes.setBestTime(transfer.targetStopIndex, comparableTargetTime);

      auto label = std::make_unique<StopLabelsAndTimes::Label>(sourceTime, targetTime, StopLabelsAndTimes::LabelType::TRANSFER, i, types::NO_INDEX, transfer.targetStopIndex, stopLabelsAndTimes.getLabel(round, stopIdx));

      stopLabelsAndTimes.setLabel(round, transfer.targetStopIndex, std::move(label));
      markedStops.insert(transfer.targetStopIndex);
    }
  }
} // raptor