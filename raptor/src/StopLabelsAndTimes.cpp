//
// Created by MichaelBrunner on 20/07/2024.
//

#include "StopLabelsAndTimes.h"

namespace raptor {
  StopLabelsAndTimes::StopLabelsAndTimes(int stopSize)
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

  std::shared_ptr<StopLabelsAndTimes::Label> StopLabelsAndTimes::getLabel(int round, int stopIdx) const {
    if (round < 0 || round >= static_cast<int>(bestLabelsPerRound.size()))
    {
      throw std::out_of_range("Round index out of range");
    }
    return bestLabelsPerRound[round][stopIdx];
  }

  void StopLabelsAndTimes::setLabel(int round, int stopIdx, std::shared_ptr<Label> label) {
    if (round < 0 || round >= static_cast<int>(bestLabelsPerRound.size()))
    {
      throw std::out_of_range("Round index out of range");
    }
    bestLabelsPerRound[round][stopIdx] = label;
  }

  int StopLabelsAndTimes::getComparableBestTime(int stopIdx) const {
    if (stopIdx < 0 || stopIdx >= static_cast<int>(bestTimeForStops.size()))
    {
      throw std::out_of_range("Stop index out of range");
    }
    return bestTimeForStops[stopIdx];
  }

  int StopLabelsAndTimes::getActualBestTime(int stopIdx) const {
    if (stopIdx < 0 || stopIdx >= static_cast<int>(bestTimeForStops.size()))
    {
      throw std::out_of_range("Stop index out of range");
    }

    for (auto it = bestLabelsPerRound.rbegin(); it != bestLabelsPerRound.rend(); ++it)
    {
      auto label = (*it)[stopIdx];
      if (label)
      {
        return label->targetTime;
      }
    }

    return std::numeric_limits<int>::max();
  }

  void StopLabelsAndTimes::setBestTime(int stopIdx, int time) {
    if (stopIdx < 0 || stopIdx >= static_cast<int>(bestTimeForStops.size()))
    {
      throw std::out_of_range("Stop index out of range");
    }
    bestTimeForStops[stopIdx] = time;
  }

  const std::vector<std::vector<std::shared_ptr<StopLabelsAndTimes::Label>>>& StopLabelsAndTimes::getBestLabelsPerRound() const {
    return bestLabelsPerRound;
  }
} // raptor