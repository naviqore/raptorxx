//
// Created by MichaelBrunner on 02/06/2024.
//

#ifndef ROUTE_H
#define ROUTE_H

#include "Stop.h"
#include <string>
#include <stdexcept>
#include <vector>
#include <cstdint>
#include <set>
#include <unordered_set>
#include <schedule_export.h>

// https://gtfs.org/schedule/reference/#routestxt
// Required

namespace schedule::gtfs {
  struct Trip;
}
namespace schedule::gtfs {
  struct SCHEDULE_API Route {
    enum RouteType : uint16_t {
      Tram = 0,
      Subway = 1,
      Rail = 2,
      Bus = 3,
      Ferry = 4,
      CableTram = 5,
      AerialLift = 6,
      Funicular = 7,
      Trolleybus = 11,
      Monorail = 12,
      Undefined = 9'999
    };

    Route(std::string aRouteId, std::string&& aRouteShortName, std::string&& aRouteLongName, const RouteType aRouteType, std::string&& aAgencyId)
      : routeId(std::move(aRouteId))
      , routeShortName(std::move(aRouteShortName))
      , routeLongName(std::move(aRouteLongName))
      , routeType(aRouteType)
      , agencyId(std::move(aAgencyId))
    {
      if (routeId.empty())
      {
        throw std::invalid_argument("Mandatory route fields must not be empty");
      }
    }
    std::string routeId;
    std::string routeShortName;
    std::string routeLongName;
    RouteType routeType;
    std::string agencyId;
    std::unordered_set<std::string> trips;
    bool isRouteActive = false;
  };

  inline auto routeHash = [](const Route& route) {
    return std::hash<std::string>{}(route.routeId);
  };

  inline auto routeEqual = [](const Route& lhs, const Route& rhs) {
    return lhs.routeId == rhs.routeId;
  };

} // gtfs

#endif //ROUTE_H
