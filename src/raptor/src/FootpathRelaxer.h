//
// Created by MichaelBrunner on 20/07/2024.
//

#ifndef FOOTPATHRELAXER_H
#define FOOTPATHRELAXER_H

#include "utils/StopLabelsAndTimes.h"
#include "usingTypes.h"
#include "include/RaptorData.h"

#include <vector>
#include <unordered_set>

namespace raptor {

  class FootpathRelaxer {
  public:
    FootpathRelaxer(StopLabelsAndTimes& stopLabelsAndTimes, const RaptorData& raptorData, types::raptorInt minimumTransferDuration, types::raptorInt maximumWalkingDuration);

    [[nodiscard]] std::unordered_set<types::raptorIdx> relaxInitial(const std::vector<types::raptorIdx>& stopIndices) const;

    [[nodiscard]] std::unordered_set<types::raptorIdx> relax(types::raptorInt round, const std::unordered_set<types::raptorIdx>& stopIndices) const;

  private:
    void expandFootpathsFromStop(types::raptorIdx stopIdx, types::raptorInt round, std::unordered_set<types::raptorInt>& markedStops) const;

    const std::vector<Transfer>& transfers;
    const std::vector<Stop>& stops;
    const types::raptorInt minTransferDuration;
    const types::raptorInt maxWalkingDuration;
    StopLabelsAndTimes& stopLabelsAndTimes;
  };

} // raptor

#endif //FOOTPATHRELAXER_H
