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

    [[nodiscard]] void relaxInitial() const;

    [[nodiscard]] void relax(int round) const;

  private:
    void expandFootpathsFromStop(types::raptorIdx stopIdx, types::raptorInt round) const;

    const std::vector<Transfer>& transfers;
    const std::vector<Stop>& stops;
    const types::raptorInt minTransferDuration;
    const types::raptorInt maxWalkingDuration;
    StopLabelsAndTimes& stopLabelsAndTimes;
  };

} // raptor

#endif //FOOTPATHRELAXER_H
