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
  StopLabelsAndTimes::~StopLabelsAndTimes() = default;

  void StopLabelsAndTimes::addNewRound() {
    bestLabelsPerRound.emplace_back(stopSize);
    for (auto& label : bestLabelsPerRound.back())
    {
      label = nullptr;
    }
  }

  const StopLabelsAndTimes::Label* StopLabelsAndTimes::getLabel(types::raptorInt const round, types::raptorIdx const stopIdx) const {
    if (round >= bestLabelsPerRound.size())
    {
      throw std::out_of_range("Round index out of range");
    }
    return bestLabelsPerRound[round][stopIdx].get();
  }

  void StopLabelsAndTimes::setLabel(types::raptorInt const round, types::raptorIdx const stopIdx, std::unique_ptr<Label>&& label) {
    if (round >= bestLabelsPerRound.size())
    {
      throw std::out_of_range("Round index out of range");
    }
    bestLabelsPerRound[round][stopIdx] = std::move(label);
  }

  types::raptorInt StopLabelsAndTimes::getComparableBestTime(types::raptorInt const stopIdx) const {
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
      if (const auto label = (*it)[stopIdx].get())
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

  const std::vector<std::vector<std::unique_ptr<StopLabelsAndTimes::Label>>>& StopLabelsAndTimes::getBestLabelsPerRound() const {
    return bestLabelsPerRound;
  }
} // raptor