//
// Created by MichaelBrunner on 13/06/2024.
//

#ifndef RELATIONMANAGER_H
#define RELATIONMANAGER_H

#include "GtfsData.h"


#include <map>
#include <vector>
#include "model/StopTime.h"
#include "model/Trip.h"
#include "model/Stop.h"
#include "model/Route.h"
#include <schedule_export.h>
#include <span>

namespace schedule::gtfs {

  class GTFS_API RelationManager
  {
  public:
    explicit RelationManager(const GtfsData& data);

    void createRelations();

    [[nodiscard]] const std::vector<StopTime>& getStopTimesForTrip(const std::string& tripId) const;

    [[nodiscard]] const std::vector<StopTime>& getStopTimesForStop(const std::string& stopId) const;

    [[nodiscard]] const std::string& getStopIdFromName(const std::string& stopName) const;

    [[nodiscard]] const std::vector<Trip>& getTripsForRoute(const std::string& routeId) const;

    [[nodiscard]] const std::vector<std::string>& getStopsForRoute(const std::string& routeId) const;

    [[nodiscard]] std::span<const StopTime> getStopTimesForRouteSpan(const std::string& routeId) const;

    [[nodiscard]] const GtfsData& getData() const;

  private:
    const GtfsData& data;

    // TODO reserve size for maps
    std::unordered_map<std::string, std::vector<StopTime>> stopTimeTrips;
    std::unordered_map<std::string, std::vector<StopTime>> stopTimeStops;
    std::unordered_map<std::string, std::vector<Trip>> tripsRoutes;
    std::unordered_map<std::string, std::vector<std::string>> routesStops;

    void collectStopTimesForTrips();

    void collectStopTimesForStops();

    void collectTripsForRoutes();

    void collectRoutesForStops();
  };

} // gtfs

#endif //RELATIONMANAGER_H
