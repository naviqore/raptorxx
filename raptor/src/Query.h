//
// Created by MichaelBrunner on 20/07/2024.
//

#ifndef QUERY_H
#define QUERY_H

#include "QueryConfig.h"
#include "utils/StopLabelsAndTimes.h"
#include "utils/RaptorData.h"

#include <vector>
#include <set>
#include <stdexcept>
#include <algorithm>
#include <memory>
#include <unordered_set>

namespace raptor {

  struct QueryParams
  {
    const RaptorData& raptorData;
    std::vector<types::raptorIdx> sourceStopIndices;
    std::vector<types::raptorIdx> targetStopIndices;
    std::vector<types::raptorInt> sourceTimes;
    std::vector<types::raptorInt> walkingDurationsToTarget;
    config::QueryConfig config;
  };

  class Query
  {
  public:

    explicit Query(const QueryParams& params);

    const std::vector<std::vector<std::unique_ptr<StopLabelsAndTimes::Label>>>& run();

  private:
    std::vector<types::raptorIdx> sourceStopIndices;
    std::vector<types::raptorIdx> targetStopIndices;
    std::vector<types::raptorInt> sourceTimes;
    std::vector<types::raptorInt> walkingDurationsToTarget;

    RaptorData raptorData;
    config::QueryConfig config;

    std::vector<types::raptorInt>targetStops;
    types::raptorInt cutoffTime;
    StopLabelsAndTimes stopLabelsAndTimes;

    std::unordered_set<types::raptorIdx> initialize();
    std::unordered_set<types::raptorIdx> removeSuboptimalLabelsForRound(types::raptorInt round, const std::unordered_set<types::raptorIdx>& markedStops);
    [[nodiscard]] types::raptorInt getBestTimeForAllTargetStops() const;
    [[nodiscard]] types::raptorInt determineCutoffTime() const;
  };

} // raptor

#endif //QUERY_H
