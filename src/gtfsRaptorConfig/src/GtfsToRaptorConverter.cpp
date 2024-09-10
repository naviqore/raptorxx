//
// Created by MichaelBrunner on 26/07/2024.
//

#include "GtfsToRaptorConverter.h"

#include "LoggerFactory.h"

namespace converter {

  GtfsToRaptorConverter::GtfsToRaptorConverter(schedule::gtfs::GtfsData&& data, const int defaultSameStopTransferTime)
    : raptorRouterBuilder(defaultSameStopTransferTime)
    , timetableManager(std::move(data))
  {
    routePartitioner = std::make_unique<RoutePartitioner>(&timetableManager.getData());
  }

  std::shared_ptr<raptor::RaptorData> GtfsToRaptorConverter::convert()
  {
    for (const auto& route : timetableManager.getRoutes()) {
      std::ranges::for_each(routePartitioner->getSubRoutes(route.routeId), [this](const SubRoute& subRoute) {
        addRoute(subRoute);
      });
    }
    addTransfers();
    return raptorRouterBuilder.build();
  }

  void GtfsToRaptorConverter::addStopIdsToRoute(std::string const& subRouteId, const std::vector<std::string>& stopIdsVector)
  {
    raptorRouterBuilder.addRoute(subRouteId, stopIdsVector);
  }

  void GtfsToRaptorConverter::addTripsToRouterBuilder(SubRoute const& subRoute)
  {
    std::unordered_set<std::string> addedTripIds; // in future use std::flat_set as cxx23 is released
    std::ranges::for_each(subRoute.getTrips(), [this, &subRoute, &addedTripIds](const schedule::gtfs::Trip* trip) {
      if (addedTripIds.contains(trip->tripId)) {
        return;
      }
      raptorRouterBuilder.addTrip(trip->tripId, subRoute.getSubRouteId());
      addedTripIds.insert(trip->tripId);

      for (const auto& stopTimes = trip->stopTimes;
           const auto [index, stopTime] : std::views::enumerate(stopTimes)) {
        addStopTimesToRouterBuilder(*stopTime, trip->tripId, subRoute.getSubRouteId(), static_cast<int>(index));
      }
    });
  }

  void GtfsToRaptorConverter::addStopTimesToRouterBuilder(schedule::gtfs::StopTime const& stopTime, std::string const& tripId, std::string const& subRouteId, const int position)
  {
    raptorRouterBuilder.addStopTime(subRouteId,
                                    tripId,
                                    position,
                                    stopTime.stopId,
                                    stopTime.arrivalTime.toSeconds(),
                                    stopTime.departureTime.toSeconds());
  }

  void GtfsToRaptorConverter::addTransfers()
  {
    for (const auto& stopId : addedStopIds) {
      for (const auto& stop = timetableManager.getData().stops.at(stopId);
           const auto transfer : stop.transferItems) {
        if (transfer->transferType == schedule::gtfs::Transfer::MINIMUM_TIME && transfer->minTransferTime > 0) {
          try {
            raptorRouterBuilder.addTransfer(transfer->fromStopId, transfer->toStopId, transfer->minTransferTime);
          }
          catch (const std::invalid_argument& e) {
            getConsoleLogger(LoggerName::RAPTOR)->error(e.what());
          }
        }
      }
    }
  }

  void GtfsToRaptorConverter::addRoute(SubRoute const& subRoute)
  {
    auto stopIds = subRoute.getStopsSequence()| std::views::transform([](const schedule::gtfs::Stop* stop) { return stop->stopId; });

    std::ranges::for_each(stopIds, [this](const std::string& stopId) {
      if (!addedStopIds.contains(stopId)) {
        raptorRouterBuilder.addStop(stopId);
        addedStopIds.insert(stopId);
      }
    });

    addStopIdsToRoute(subRoute.getSubRouteId(), std::vector<std::string>{stopIds.begin(), stopIds.end()});
    addTripsToRouterBuilder(subRoute);
  }
} // converter
