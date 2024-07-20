//
// Created by MichaelBrunner on 20/07/2024.
//

#ifndef QUERY_H
#define QUERY_H

#include "QueryConfig.h"
#include "StopLabelsAndTimes.h"
#include "utils/RaptorData.h"

#include <vector>
#include <set>
#include <stdexcept>
#include <algorithm>
#include <memory>
#include <unordered_set>

namespace raptor {

  class Query {
  public:
    Query(const RaptorData& raptorData, const std::vector<int>& sourceStopIndices, const std::vector<int>& targetStopIndices, const std::vector<int>& sourceTimes, const std::vector<int>& walkingDurationsToTarget, const QueryConfig& config);

    std::vector<std::vector<std::shared_ptr<StopLabelsAndTimes::Label>>> run();

  private:
    std::vector<int> sourceStopIndices;
    std::vector<int> targetStopIndices;
    std::vector<int> sourceTimes;
    std::vector<int> walkingDurationsToTarget;

    RaptorData raptorData;
    QueryConfig config;

    std::vector<int> targetStops;
    int cutoffTime;
    StopLabelsAndTimes stopLabelsAndTimes;

    std::unordered_set<int> initialize();
    std::unordered_set<int> removeSuboptimalLabelsForRound(int round, const std::unordered_set<int>& markedStops);
    int getBestTimeForAllTargetStops() const;
    int determineCutoffTime() const;
  };

} // raptor

#endif //QUERY_H
