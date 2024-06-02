//
// Created by MichaelBrunner on 02/06/2024.
//

#ifndef ROUTE_H
#define ROUTE_H

#include <string>
#include <stdexcept>

// https://gtfs.org/schedule/reference/#routestxt
// Required

namespace gtfs {
  struct Route
  {
    Route(std::string&& aRouteId, std::string&& aRouteShortName, std::string&& aRouteLongName, const int aRouteType)
      : routeId(std::move(aRouteId))
      , routeShortName(std::move(aRouteShortName))
      , routeLongName(std::move(aRouteLongName))
      , routeType(aRouteType) {
      if (routeId.empty()
          || routeShortName.empty()
          || routeLongName.empty())
      {
        throw std::invalid_argument("Mandatory route fields must not be empty");
      }
    }
    std::string routeId;
    std::string routeShortName;
    std::string routeLongName;
    int routeType;
    // std::string agency_id;
  };

} // gtfs

#endif //ROUTE_H
