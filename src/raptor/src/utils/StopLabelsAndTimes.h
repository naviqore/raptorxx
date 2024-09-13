//
// Created by MichaelBrunner on 20/07/2024.
//

#ifndef STOPLABELSANDTIMES_H
#define STOPLABELSANDTIMES_H


#include "usingTypes.h"


#include <utility>
#include <vector>
#include <memory>
#include <limits>
#include <stdexcept>
#include <algorithm>

namespace raptor {

  class StopLabelsAndTimes {
  public:
    enum class LabelType {
      INITIAL,
      ROUTE,
      TRANSFER
    };

    struct Label {
      types::raptorInt sourceTime;
      types::raptorInt targetTime;
      LabelType type;
      types::raptorIdx routeOrTransferIdx;
      int tripOffset;
      types::raptorIdx stopIdx;
      const Label* previous;

      Label(const types::raptorInt sourceTime, const types::raptorInt targetTime, const LabelType type, const types::raptorIdx routeOrTransferIdx, const int tripOffset, const types::raptorIdx stopIdx, const Label* previous = nullptr)
        : sourceTime(sourceTime)
        , targetTime(targetTime)
        , type(type)
        , routeOrTransferIdx(routeOrTransferIdx)
        , tripOffset(tripOffset)
        , stopIdx(stopIdx)
        , previous(previous)
      {
      }

      Label(const Label&) = delete;
      Label& operator=(const Label&) = delete;
    };

    explicit StopLabelsAndTimes(int stopSize);
    ~StopLabelsAndTimes();

    void addNewRound();
    [[nodiscard]] const Label* getLabel(types::raptorInt round, types::raptorIdx stopIdx) const;
    void setLabel(types::raptorInt round, types::raptorIdx stopIdx, std::unique_ptr<Label>&& label);
    [[nodiscard]] types::raptorInt getComparableBestTime(types::raptorIdx stopIdx) const;
    [[nodiscard]] types::raptorInt getActualBestTime(types::raptorIdx stopIdx) const;
    void setBestTime(types::raptorIdx stopIdx, types::raptorInt time);
    [[nodiscard]] const std::vector<std::vector<std::unique_ptr<Label>>>& getBestLabelsPerRound() const;

  private:
    std::vector<std::vector<std::unique_ptr<Label>>> bestLabelsPerRound;
    std::vector<types::raptorInt> bestTimeForStops;
    int stopSize;
  };

} // raptor

#endif //STOPLABELSANDTIMES_H
