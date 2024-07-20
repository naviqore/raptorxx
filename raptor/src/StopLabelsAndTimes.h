//
// Created by MichaelBrunner on 20/07/2024.
//

#ifndef STOPLABELSANDTIMES_H
#define STOPLABELSANDTIMES_H


#include <utility>
#include <vector>
#include <memory>
#include <limits>
#include <stdexcept>
#include <algorithm>

namespace raptor {

  class StopLabelsAndTimes {
  public:
    static const int NO_INDEX = -1;

    enum class LabelType {
      INITIAL,
      ROUTE,
      TRANSFER
  };

    struct Label {
      int sourceTime;
      int targetTime;
      LabelType type;
      int routeOrTransferIdx;
      int tripOffset;
      int stopIdx;
      std::shared_ptr<Label> previous;

      Label(int sourceTime, int targetTime, LabelType type, int routeOrTransferIdx, int tripOffset, int stopIdx,
            std::shared_ptr<Label> previous = nullptr)
          : sourceTime(sourceTime), targetTime(targetTime), type(type), routeOrTransferIdx(routeOrTransferIdx),
            tripOffset(tripOffset), stopIdx(stopIdx), previous(std::move(previous)) {}
    };

    StopLabelsAndTimes(int stopSize);

    void addNewRound();
    std::shared_ptr<Label> getLabel(int round, int stopIdx) const;
    void setLabel(int round, int stopIdx, std::shared_ptr<Label> label);
    int getComparableBestTime(int stopIdx) const;
    int getActualBestTime(int stopIdx) const;
    void setBestTime(int stopIdx, int time);
    const std::vector<std::vector<std::shared_ptr<Label>>>& getBestLabelsPerRound() const;

  private:
    std::vector<std::vector<std::shared_ptr<Label>>> bestLabelsPerRound;
    std::vector<int> bestTimeForStops;
    int stopSize;
  };

} // raptor

#endif //STOPLABELSANDTIMES_H
