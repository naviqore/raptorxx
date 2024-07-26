//
// Created by MichaelBrunner on 20/07/2024.
//

#ifndef ROUTESCANNER_H
#define ROUTESCANNER_H

#include "ActiveTrip.h"
#include "StopLabelsAndTimes.h"
#include "utils/RaptorData.h"

#include <unordered_set>
#include <utility>
#include <vector>
#include <memory>
#include <stdexcept>
#include <iostream>

namespace raptor {


  class RouteScanner {

  public:

    RouteScanner(const StopLabelsAndTimes& stopLabelsAndTimes, const RaptorData& raptorData,
                 types::raptorInt minimumTransferDuration);

    std::unordered_set<types::raptorIdx> scan(types::raptorInt round, const std::unordered_set<types::raptorIdx>& markedStops);

  private:
    [[nodiscard]] std::unordered_set<types::raptorIdx> getRoutesToScan(const std::unordered_set<types::raptorIdx>& markedStops) const;
    void scanRoute(types::raptorIdx currentRouteIdx, types::raptorInt round, const std::unordered_set<types::raptorIdx>& markedStops, std::unordered_set<types::raptorIdx>& markedStopsNext);

    bool canEnterAtStop(const Stop& stop, types::raptorInt stopTime, const std::unordered_set<types::raptorIdx>& markedStops,
                        types::raptorIdx stopIdx, types::raptorInt stopOffset, types::raptorInt numberOfStops);

    bool checkIfTripIsPossibleAndUpdateMarks(const StopTime& stopTime, const std::shared_ptr<ActiveTrip>& activeTrip, const Stop& stop, types::raptorInt bestStopTime,
      types::raptorIdx stopIdx, types::raptorInt thisRound, types::raptorInt lastRound, std::unordered_set<types::raptorIdx>& markedStopsNext, types::raptorIdx currentRouteIdx) const;

    [[nodiscard]] std::shared_ptr<ActiveTrip> findPossibleTrip(types::raptorIdx stopIdx, const Stop& stop, types::raptorInt stopOffset, const Route& route, types::raptorInt lastRound) const;

    const std::vector<Stop>& stops;
    const std::vector<types::raptorIdx>& stopRoutes;
    const std::vector<StopTime>& stopTimes;
    const std::vector<Route>& routes;
    const std::vector<RouteStop>& routeStops;

    StopLabelsAndTimes& stopLabelsAndTimes;

    const types::raptorInt minTransferDuration;

  };

} // raptor

#endif //ROUTESCANNER_H
