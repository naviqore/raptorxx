//
// Created by MichaelBrunner on 02/06/2024.
//

#ifndef ROUTE_H
#define ROUTE_H

#include <string>
#include <stdexcept>
#include <cstdint>

// https://gtfs.org/schedule/reference/#routestxt
// Required

namespace schedule::gtfs {
  struct Route
  {
    enum RouteType : uint8_t
    {
      Tram = 0,
      Subway = 1,
      Rail = 2,
      Bus = 3,
      Ferry = 4,
      CableTram = 5,
      AerialLift = 6,
      Funicular = 7,
      Trolleybus = 11,
      Monorail = 12
    };

    Route(std::string&& aRouteId, std::string&& aRouteShortName, std::string&& aRouteLongName, const RouteType aRouteType)
      : routeId(std::move(aRouteId))
      , routeShortName(std::move(aRouteShortName))
      , routeLongName(std::move(aRouteLongName))
      , routeType(aRouteType) {
      if (routeId.empty()) // || routeShortName.empty()  || routeLongName.empty()
      {
        throw std::invalid_argument("Mandatory route fields must not be empty");
      }
      if (routeType > 12)
      {
        // TODO Log error - there are some route types that are not defined in the GTFS standard
        // throw std::invalid_argument("Invalid route type");
      }
    }
    std::string routeId;
    std::string routeShortName;
    std::string routeLongName;
    RouteType routeType;
    // std::string agency_id;
  };

} // gtfs

#endif //ROUTE_H
