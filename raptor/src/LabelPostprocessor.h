//
// Created by MichaelBrunner on 02/08/2024.
//

#pragma once

#include "Connection.h"
#include "RaptorConnection.h"
#include "raptorRouteStructures.h"
#include "usingTypes.h"
#include "utils/StopLabelsAndTimes.h"

#include <map>
#include <vector>

enum class TimeType
{
  DEPARTURE,
  ARRIVAL
};

namespace raptor {
  class RaptorRouter;

  class LabelPostprocessor
  {
    std::vector<Stop> stops;
    std::vector<StopTime> stopTimes;
    std::vector<Route> routes;
    std::vector<RouteStop> routeStops;

    std::unique_ptr<Connection> reconstructConnectionFromLabel(const StopLabelsAndTimes::Label* label, const types::raptorInt& referenceDate);
    void maybeCombineFirstTwoLabels(std::vector<const StopLabelsAndTimes::Label*>& labels);
    void maybeCombineLastTwoLabels(std::vector<const StopLabelsAndTimes::Label*>& labels);
    void maybeCombineLabels(std::vector<const StopLabelsAndTimes::Label*>& labels, bool fromTarget);
    void maybeShiftSourceTransferCloserToFirstRoute(std::vector<const StopLabelsAndTimes::Label*>& labels,
                                                    const StopLabelsAndTimes::Label& transferLabel,
                                                    const StopLabelsAndTimes::Label& routeLabel,
                                                    int transferLabelIndex);
    std::optional<StopTime> getTripStopTimeForStopInTrip(int stopIdx, int routeIdx, int tripOffset);
    std::optional<StopLabelsAndTimes::Label> getBestLabelForStop(const std::vector<std::vector<StopLabelsAndTimes::Label>>& bestLabelsPerRound, int stopIdx);

  public:
    explicit LabelPostprocessor(const RaptorRouter& raptorData);
    std::map<std::string, std::unique_ptr<Connection>> reconstructIsolines(const std::vector<std::vector<StopLabelsAndTimes::Label>>& bestLabelsPerRound, const types::raptorInt& referenceDate) const;
    std::vector<std::unique_ptr<Connection>> reconstructParetoOptimalSolutions(const std::vector<std::vector<std::unique_ptr<StopLabelsAndTimes::Label>>>& bestLabelsPerRound,
                                                                               const std::map<types::raptorIdx, types::raptorIdx>& targetStops,
                                                                               const types::raptorInt& referenceDate);
  };

} // raptor
