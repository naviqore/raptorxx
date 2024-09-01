//
// Created by MichaelBrunner on 19/07/2024.
//

#pragma once

#include "GtfsData.h"
#include "model/Route.h"
#include <gtfsRaptorConfig_export.h>
#include <unordered_map>
#include <vector>
#include <string>
#include "SubRoute.h"

namespace converter {

  class GTFS_RAPTOR_API RoutePartitioner
  {

  public:
    explicit RoutePartitioner(schedule::gtfs::GtfsData* data);

    [[nodiscard]] std::vector<SubRoute> getSubRoutes(std::string const& routeId) const;

    [[nodiscard]] const SubRoute& getSubRoute(std::string const& tripId) const;

    [[nodiscard]] const std::vector<std::string>& getTrips(std::string const& routeId) const;


  private:
    std::unordered_map<std::string, std::unordered_map<std::string, SubRoute> /*, decltype(routeHash), decltype(routeEqual)*/> subRoutes{};
    schedule::gtfs::GtfsData* data;

    void processRoute(schedule::gtfs::Route const& route);

    [[nodiscard]] std::string generateStopSequenceKey(const std::string& tripId) const;

    [[nodiscard]] std::vector<schedule::gtfs::Stop> extractStopSequence(schedule::gtfs::Trip const& aTrip) const;
  };

} // gtfs
// schedule
