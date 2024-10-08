//
// Created by MichaelBrunner on 02/08/2024.
//

#pragma once

#include "GtfsData.h"

#include <functional>
#include <gtfsRaptorConfig_export.h>

namespace converter {

  class GTFS_RAPTOR_API SubRoute {
    std::string SubRouteId;
    std::string routeId;
    std::string stopSequenceKey;
    std::vector<const schedule::gtfs::Stop*> stopsSequence; // brave attempt with dangling pointer, but we know that the objects are alive and have performance in focus
    std::vector<const schedule::gtfs::Trip*> trips; // brave attempt with dangling pointer, but we know that the objects are alive and have performance in focus

  public:
    explicit SubRoute(std::string&& subRouteId, std::string routeId, std::string stopSequenceKey, std::vector<const schedule::gtfs::Stop*>&& stopsSequence);

    SubRoute(const SubRoute& aSubRoute);

    SubRoute(SubRoute&& aSubRoute) noexcept = default;
    SubRoute& operator=(const SubRoute& aSubRoute) = default;
    SubRoute& operator=(SubRoute&& aSubRoute) noexcept = default;

    void addTrip(schedule::gtfs::Trip const& trip);

    [[nodiscard]] const std::string& getSubRouteId() const;

    [[nodiscard]] const std::string& getRouteId() const;

    [[nodiscard]] const std::string& getStopSequenceKey() const;

    [[nodiscard]] const std::vector<const schedule::gtfs::Stop*>& getStopsSequence() const;

    [[nodiscard]] size_t stopIndex(std::string_view stopId) const;

    [[nodiscard]] const std::vector<const schedule::gtfs::Trip*>& getTrips() const;

    bool operator==(const SubRoute& aSubRoute) const;

    std::function<size_t(const SubRoute&)> SubRouteHash = [](const SubRoute& aSubRoute) {
      return std::hash<std::string>{}(aSubRoute.getSubRouteId());
    };

    std::function<bool(const SubRoute&, const SubRoute&)> SubRouteEqual = [](const SubRoute& aSubRoute, const SubRoute& anotherSubRoute) {
      return aSubRoute == anotherSubRoute;
    };
  };



} // converter
