//
// Created by MichaelBrunner on 20/07/2024.
//

#include "StopLabelsAndTimes.h"

namespace raptor {
  StopLabelsAndTimes::StopLabelsAndTimes(const int stopSize)
    : stopSize(stopSize)
    , bestTimeForStops(stopSize) {

    // Initialize best times to initial value
    std::ranges::fill(bestTimeForStops, std::numeric_limits<int>::max());


    // Set empty labels for first round
    addNewRound();
  }

  void StopLabelsAndTimes::addNewRound() {
    bestLabelsPerRound.emplace_back(stopSize, nullptr);
  }

  std::shared_ptr<StopLabelsAndTimes::Label> StopLabelsAndTimes::getLabel(types::raptorInt const round, types::raptorIdx const  stopIdx) const {
    if (round >=bestLabelsPerRound.size())
    {
      throw std::out_of_range("Round index out of range");
    }
    return bestLabelsPerRound[round][stopIdx];
  }

  void StopLabelsAndTimes::setLabel(types::raptorInt const round, types::raptorIdx const stopIdx, const std::shared_ptr<Label>& label) {
    if (round >= bestLabelsPerRound.size())
    {
      throw std::out_of_range("Round index out of range");
    }
    bestLabelsPerRound[round][stopIdx] = label;
  }

  types::raptorInt StopLabelsAndTimes::getComparableBestTime(types::raptorInt const  stopIdx) const {
    if (stopIdx >= bestTimeForStops.size())
    {
      throw std::out_of_range("Stop index out of range");
    }
    return bestTimeForStops[stopIdx];
  }

  types::raptorInt StopLabelsAndTimes::getActualBestTime(types::raptorIdx const stopIdx) const {
    if (stopIdx >= bestTimeForStops.size())
    {
      throw std::out_of_range("Stop index out of range");
    }

    for (auto it = bestLabelsPerRound.rbegin(); it != bestLabelsPerRound.rend(); ++it)
    {
      if (const auto label = (*it)[stopIdx])
      {
        return label->targetTime;
      }
    }

    return types::INFINITY_VALUE_MAX;
  }

  void StopLabelsAndTimes::setBestTime(const types::raptorIdx stopIdx, const types::raptorInt time) {
    if (stopIdx >= bestTimeForStops.size())
    {
      throw std::out_of_range("Stop index out of range");
    }
    bestTimeForStops[stopIdx] = time;
  }

  const std::vector<std::vector<std::shared_ptr<StopLabelsAndTimes::Label>>>& StopLabelsAndTimes::getBestLabelsPerRound() const {
    return bestLabelsPerRound;
  }
} // raptor