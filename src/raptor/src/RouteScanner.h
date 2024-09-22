//
// Created by MichaelBrunner on 20/07/2024.
//

#ifndef ROUTESCANNER_H
#define ROUTESCANNER_H

#include "data/ActiveTrip.h"
#include "utils/StopLabelsAndTimes.h"
#include "include/RaptorData.h"

#include <unordered_set>
#include <utility>
#include <vector>
#include <memory>
#include <stdexcept>
#include <iostream>

namespace raptor {

  class RouteScanner
  {
  public:
    RouteScanner(StopLabelsAndTimes& stopLabelsAndTimes, const RaptorData& raptorData, types::raptorInt minimumTransferDuration);

    void scan(int round);

  private:
    [[nodiscard]] std::unordered_set<types::raptorIdx> getRoutesToScan(const std::vector<bool>& markedStops) const;

    void scanRoute(types::raptorIdx currentRouteIdx, types::raptorInt round) const;

    bool checkIfTripIsPossibleAndUpdateMarks(const StopTime& stopTime, const ActiveTrip& activeTrip, const Stop& stop, types::raptorInt bestStopTime, types::raptorIdx stopIdx, types::raptorInt thisRound, types::raptorInt lastRound, types::raptorIdx currentRouteIdx) const;

    [[nodiscard]] std::optional<ActiveTrip> findPossibleTrip(types::raptorIdx stopIdx, const Stop& stop, types::raptorInt stopOffset, const Route& route, types::raptorInt lastRound) const;

    void setRoutesToScan();

    const std::vector<Stop>& stops;
    const std::vector<types::raptorIdx>& stopRoutes;
    const std::vector<StopTime>& stopTimes;
    const std::vector<Route>& routes;
    const std::vector<RouteStop>& routeStops;
    StopLabelsAndTimes& stopLabelsAndTimes;
    const types::raptorInt minTransferDuration;
    std::vector<bool> routesToScanMask;
  };

} // raptor

#endif //ROUTESCANNER_H
