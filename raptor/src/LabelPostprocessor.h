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

namespace raptor {
  class RaptorRouter;

  class LabelPostprocessor
  {
    std::vector<Stop> stops;
    std::vector<StopTime> stopTimes;
    std::vector<Route> routes;
    std::vector<RouteStop> routeStops;

    std::unique_ptr<Connection> reconstructConnectionFromLabel(const StopLabelsAndTimes::Label* label, const types::raptorInt& referenceDate) const;

  public:
    explicit LabelPostprocessor(const RaptorRouter& raptorData);
    std::vector<std::unique_ptr<Connection>> reconstructParetoOptimalSolutions(const std::vector<std::vector<std::unique_ptr<StopLabelsAndTimes::Label>>>& bestLabelsPerRound,
                                                                               const std::map<types::raptorIdx, types::raptorIdx>& targetStops,
                                                                               const types::raptorInt& referenceDate) const;
  };

} // raptor
