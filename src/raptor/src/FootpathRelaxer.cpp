//
// Created by MichaelBrunner on 20/07/2024.
//

#include "FootpathRelaxer.h"

#include "LoggerFactory.h"
#include "usingTypes.h"

#include <algorithm>
#include <iostream>
#include <format>

#include "utils/scopedTimer.h"

namespace raptor {

  FootpathRelaxer::FootpathRelaxer(StopLabelsAndTimes& stopLabelsAndTimes, const RaptorData& raptorData, const types::raptorInt minimumTransferDuration, const types::raptorInt maximumWalkingDuration)
    : transfers(raptorData.getStopContext().transfers)
    , stops(raptorData.getStopContext().stops)
    , minTransferDuration(minimumTransferDuration)
    , maxWalkingDuration(maximumWalkingDuration)
    , stopLabelsAndTimes(stopLabelsAndTimes)
  {
  }

  void FootpathRelaxer::relaxInitial() const
  {
    // MEASURE_FUNCTION();

    relax(0);
  }

  void FootpathRelaxer::relax(const int round) const
  {

#if LOGGER
    getConsoleLogger(LoggerName::RAPTOR)->info(std::format("Relaxing footpaths for round {}", round));
#endif

    auto routeMarkedStops = stopLabelsAndTimes.getMarkedStopsMaskNextRound();
    for (auto sourceStopIndex{0}; sourceStopIndex < routeMarkedStops.size(); sourceStopIndex++) {
      if (!routeMarkedStops[sourceStopIndex]) {
        continue;
      }

      expandFootpathsFromStop(sourceStopIndex, round);
    }
  }

  void FootpathRelaxer::expandFootpathsFromStop(const types::raptorIdx stopIdx, const types::raptorInt round) const
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
#if LOGGER
      getConsoleLogger(LoggerName::RAPTOR)->info(std::format("Stop {} was improved by transfer from stop {}", stop.id, targetStop.id));
#endif


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

      stopLabelsAndTimes.mark(transfer.targetStopIndex);
    }
  }
} // raptor
