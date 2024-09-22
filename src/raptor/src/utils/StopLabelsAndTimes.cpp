//
// Created by MichaelBrunner on 20/07/2024.
//

#include "StopLabelsAndTimes.h"

#include <ranges>

namespace raptor {
  StopLabelsAndTimes::StopLabelsAndTimes(const int stopSize)
    : bestTimeForStops(stopSize)
    , stopSize(stopSize)
  {

    // Initialize best times to initial value
    std::ranges::fill(bestTimeForStops, std::numeric_limits<int>::max());

    this->markedStopsMaskNextRound.resize(stopSize);
    this->markedStopsMaskThisRound.resize(stopSize);

    std::ranges::fill(markedStopsMaskThisRound, false);
    std::ranges::fill(markedStopsMaskNextRound, false);

    // Set empty labels for first round
    addNewRound();
  }
  StopLabelsAndTimes::~StopLabelsAndTimes() = default;

  void StopLabelsAndTimes::addNewRound()
  {
    std::swap(markedStopsMaskThisRound, markedStopsMaskNextRound);
    std::ranges::fill(markedStopsMaskNextRound, false);

    this->round++;

    bestLabelsPerRound.emplace_back(stopSize);
    for (auto& label : bestLabelsPerRound.back()) {
      label = nullptr;
    }
  }

  const StopLabelsAndTimes::Label* StopLabelsAndTimes::getLabel(types::raptorInt const round, types::raptorIdx const stopIdx) const
  {
    if (round >= bestLabelsPerRound.size()) {
      throw std::out_of_range("Round index out of range");
    }
    return bestLabelsPerRound[round][stopIdx].get();
  }

  void StopLabelsAndTimes::setLabel(types::raptorInt const round, types::raptorIdx const stopIdx, std::unique_ptr<Label>&& label)
  {
    if (round >= bestLabelsPerRound.size()) {
      throw std::out_of_range("Round index out of range");
    }
    bestLabelsPerRound[round][stopIdx] = std::move(label);
  }

  types::raptorInt StopLabelsAndTimes::getComparableBestTime(types::raptorInt const stopIdx) const
  {
    if (stopIdx >= bestTimeForStops.size()) {
      throw std::out_of_range("Stop index out of range");
    }
    return bestTimeForStops[stopIdx];
  }

  types::raptorInt StopLabelsAndTimes::getActualBestTime(types::raptorIdx const stopIdx) const
  {
    if (stopIdx >= bestTimeForStops.size()) {
      throw std::out_of_range("Stop index out of range");
    }

    for (const auto& it : std::ranges::reverse_view(bestLabelsPerRound)) {
      if (const auto label = it[stopIdx].get()) {
        return label->targetTime;
      }
    }

    return types::INFINITY_VALUE_MAX;
  }

  void StopLabelsAndTimes::setBestTime(const types::raptorIdx stopIdx, const types::raptorInt time)
  {
    if (stopIdx >= bestTimeForStops.size()) {
      throw std::out_of_range("Stop index out of range");
    }
    bestTimeForStops[stopIdx] = time;
  }

  const std::vector<std::vector<std::unique_ptr<StopLabelsAndTimes::Label>>>& StopLabelsAndTimes::getBestLabelsPerRound() const
  {
    return bestLabelsPerRound;
  }

  bool StopLabelsAndTimes::isMarkedThisRound(const types::raptorIdx stopIdx) const
  {
    return markedStopsMaskThisRound[stopIdx];
  }
  bool StopLabelsAndTimes::isMarkedNextRound(const types::raptorIdx stopIdx) const
  {
    return markedStopsMaskNextRound[stopIdx];
  }
  void StopLabelsAndTimes::mark(const types::raptorIdx stopIdx)
  {
    markedStopsMaskNextRound[stopIdx] = true;
  }
  void StopLabelsAndTimes::unmark(const types::raptorIdx stopIdx)
  {
    markedStopsMaskNextRound[stopIdx] = false;
  }
  bool StopLabelsAndTimes::hasMarkedStops() const
  {
    for (auto i{0}; i < stopSize; ++i) {
      if (markedStopsMaskNextRound[i]) {
        return true;
      }
    }
    return false;
  }

  const std::vector<bool>& StopLabelsAndTimes::getMarkedStopsMaskNextRound() const
  {
    return markedStopsMaskNextRound;
  }

  int StopLabelsAndTimes::getRound() const
  {
    return round;
  }
} // raptor
