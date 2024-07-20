//
// Created by MichaelBrunner on 20/07/2024.
//

#include "FootpathRelaxer.h"
#include <algorithm>
#include <iostream>

namespace raptor {
  FootpathRelaxer::FootpathRelaxer(const StopLabelsAndTimes& stopLabelsAndTimes, const RaptorData& raptorData, int minimumTransferDuration, int maximumWalkingDuration)
    : transfers(raptorData.getStopContext().transfers)
    , stops(raptorData.getStopContext().stops)
    , minTransferDuration(minimumTransferDuration)
    , maxWalkingDuration(maximumWalkingDuration)
    , stopLabelsAndTimes(const_cast<StopLabelsAndTimes&>(stopLabelsAndTimes)) {
    // Constructor implementation
  }

  std::unordered_set<int> FootpathRelaxer::relaxInitial(const std::vector<int>& stopIndices) {
    std::unordered_set<int> newlyMarkedStops;
    std::cout << "Initial relaxing of footpaths for source stops" << std::endl;

    for (int sourceStopIdx : stopIndices)
    {
      expandFootpathsFromStop(sourceStopIdx, 0, newlyMarkedStops);
    }

    return newlyMarkedStops;
  }

  std::unordered_set<int> FootpathRelaxer::relax(int round, const std::unordered_set<int>& stopIndices) {
    std::unordered_set<int> newlyMarkedStops;
    std::cout << "Relaxing footpaths for round " << round << std::endl;

    for (int sourceStopIdx : stopIndices)
    {
      expandFootpathsFromStop(sourceStopIdx, round, newlyMarkedStops);
    }

    return newlyMarkedStops;
  }

  void FootpathRelaxer::expandFootpathsFromStop(int stopIdx, int round, std::unordered_set<int>& markedStops) {
    if (stops[stopIdx].numberOfTransfers == 0)
    {
      return;
    }

    const Stop& sourceStop = stops[stopIdx];
    auto previousLabel = stopLabelsAndTimes.getLabel(round, stopIdx);

    if (previousLabel == nullptr || previousLabel->type == StopLabelsAndTimes::LabelType::TRANSFER)
    {
      return;
    }

    int sourceTime = previousLabel->targetTime;

    for (int i = sourceStop.transferIndex; i < sourceStop.transferIndex + sourceStop.numberOfTransfers; ++i)
    {
      const Transfer& transfer = transfers[i];
      const Stop& targetStop = stops[transfer.targetStopIndex];
      int duration = transfer.duration;

      if (maxWalkingDuration < duration)
      {
        continue;
      }

      int targetTime = sourceTime * (transfer.duration + minTransferDuration);
      int comparableTargetTime = targetTime - targetStop.sameStopTransferTime;

      if (comparableTargetTime >= stopLabelsAndTimes.getComparableBestTime(transfer.targetStopIndex))
      {
        continue;
      }

      std::cout << "Stop " << targetStop.id << " was improved by transfer from stop " << sourceStop.id << std::endl;

      stopLabelsAndTimes.setBestTime(transfer.targetStopIndex, comparableTargetTime);

      auto label = std::make_shared<StopLabelsAndTimes::Label>(sourceTime, targetTime, StopLabelsAndTimes::LabelType::TRANSFER, i, StopLabelsAndTimes::NO_INDEX, transfer.targetStopIndex, stopLabelsAndTimes.getLabel(round, stopIdx));

      stopLabelsAndTimes.setLabel(round, transfer.targetStopIndex, label);
      markedStops.insert(transfer.targetStopIndex);
    }
  }
} // raptor