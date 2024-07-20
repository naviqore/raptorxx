//
// Created by MichaelBrunner on 20/07/2024.
//

#ifndef FOOTPATHRELAXER_H
#define FOOTPATHRELAXER_H

#include "StopLabelsAndTimes.h"
#include "utils/RaptorData.h"

#include <vector>
#include <unordered_set>
#include <stdexcept>

namespace raptor {

  class FootpathRelaxer {
  public:
    FootpathRelaxer(const StopLabelsAndTimes& stopLabelsAndTimes, const RaptorData& raptorData,
                    int minimumTransferDuration, int maximumWalkingDuration);

    std::unordered_set<int> relaxInitial(const std::vector<int>& stopIndices);
    std::unordered_set<int> relax(int round, const std::unordered_set<int>& stopIndices);

  private:
    void expandFootpathsFromStop(int stopIdx, int round, std::unordered_set<int>& markedStops);

    const std::vector<Transfer>& transfers;
    const std::vector<Stop>& stops;
    const int minTransferDuration;
    const int maxWalkingDuration;
    StopLabelsAndTimes& stopLabelsAndTimes;
  };

} // raptor

#endif //FOOTPATHRELAXER_H
