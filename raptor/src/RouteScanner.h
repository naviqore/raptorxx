//
// Created by MichaelBrunner on 20/07/2024.
//

#ifndef ROUTESCANNER_H
#define ROUTESCANNER_H

#include "StopLabelsAndTimes.h"
#include "utils/RaptorData.h"

#include <unordered_set>
#include <vector>
#include <memory>
#include <stdexcept>
#include <iostream>

namespace raptor {


  struct ActiveTrip
  {
    int tripOffset;
    int entryTime;
    std::shared_ptr<StopLabelsAndTimes::Label> previousLabel;
  };

  class RouteScanner {

  public:

    struct ActiveTrip {
      int tripOffset;
      int entryTime;
      std::shared_ptr<StopLabelsAndTimes::Label> previousLabel;

      ActiveTrip(int tripOffset, int entryTime, std::shared_ptr<StopLabelsAndTimes::Label> previousLabel)
          : tripOffset(tripOffset), entryTime(entryTime), previousLabel(previousLabel) {}
    };

    RouteScanner(const StopLabelsAndTimes& stopLabelsAndTimes, const RaptorData& raptorData,
                 int minimumTransferDuration);

    std::unordered_set<int> scan(int round, const std::unordered_set<int>& markedStops);

  private:
    std::unordered_set<int> getRoutesToScan(const std::unordered_set<int>& markedStops);
    void scanRoute(int currentRouteIdx, int round, const std::unordered_set<int>& markedStops,
                   std::unordered_set<int>& markedStopsNext);

    bool canEnterAtStop(const Stop& stop, int stopTime, const std::unordered_set<int>& markedStops,
                        int stopIdx, int stopOffset, int numberOfStops);

    bool checkIfTripIsPossibleAndUpdateMarks(const StopTime& stopTime, std::shared_ptr<::raptor::RouteScanner::ActiveTrip> activeTrip, const Stop& stop, int bestStopTime, int stopIdx, int thisRound, int lastRound, std::unordered_set<int>& markedStopsNext, int currentRouteIdx);

    std::shared_ptr<raptor::RouteScanner::ActiveTrip> findPossibleTrip(int stopIdx, const Stop& stop, int stopOffset, const Route& route, int lastRound);

    const std::vector<Stop>& stops;
    const std::vector<int>& stopRoutes;
    const std::vector<StopTime>& stopTimes;
    const std::vector<Route>& routes;
    const std::vector<RouteStop>& routeStops;

    StopLabelsAndTimes& stopLabelsAndTimes;

    const int minTransferDuration;

  };

} // raptor

#endif //ROUTESCANNER_H
