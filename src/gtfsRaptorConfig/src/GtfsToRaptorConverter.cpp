//
// Created by MichaelBrunner on 26/07/2024.
//

#include "GtfsToRaptorConverter.h"

namespace converter {

  GtfsToRaptorConverter::GtfsToRaptorConverter(schedule::gtfs::GtfsData&& data, const int defaultSameStopTransferTime)
    : raptorRouterBuilder(defaultSameStopTransferTime)
    , timetableManager(std::move(data))
    , routePartitioner(&timetableManager.getData()) //TODO: not so nice since data is moved into timetableManager - create const reference ctor in TimetableManager
  {
  }

  std::shared_ptr<raptor::RaptorData> GtfsToRaptorConverter::convert()
  {
    for (const auto& route : timetableManager.getRoutes()) {
      std::ranges::for_each(routePartitioner.getSubRoutes(route.routeId), [this](const SubRoute& subRoute) {

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
    std::ranges::for_each(subRoute.getTrips(), [this, &subRoute](const schedule::gtfs::Trip& trip) {
      raptorRouterBuilder.addTrip(trip.tripId, subRoute.getRouteId());
      const auto& stopTimes = trip.stopTimes;
      for (auto i = 0; i < stopTimes.size() - 1; ++i) {
        const auto& stopTime = stopTimes[i];
        addStopTimesToRouterBuilder(stopTime, trip.tripId, i);
      }
    });
  }

  void GtfsToRaptorConverter::addStopTimesToRouterBuilder(schedule::gtfs::StopTime const& stopTime, std::string const& tripId, const int position)
  {
    raptorRouterBuilder.addStopTime(stopTime.stopId,
                                    tripId,
                                    position,
                                    stopTime.stopId,
                                    stopTime.arrivalTime.toSeconds(),
                                    stopTime.departureTime.toSeconds());
  }
  void GtfsToRaptorConverter::addTransfers()
  {
    std::ranges::for_each(addedStopIds, [this](const std::string& stopId) {
      std::ranges::for_each(timetableManager.getData().transfer.at(stopId), [this, &stopId](const schedule::gtfs::Transfer& transfer) {
        raptorRouterBuilder.addTransfer(stopId, transfer.toStopId, transfer.minTransferTime);
      });
    });
  }

  void GtfsToRaptorConverter::addRoute(SubRoute const& subRoute)
  {
    auto stopIds = subRoute.getStopsSequence()
                   | std::views::transform([](const schedule::gtfs::Stop& stop) { return stop.stopId; });

    std::ranges::for_each(stopIds, [this](const std::string& stopId) {
      if (!addedStopIds.contains(stopId)) {
        raptorRouterBuilder.addStop(stopId);
        addedStopIds.insert(stopId);
      }
    });

    addStopIdsToRoute(subRoute.getRouteId(), std::vector<std::string>{stopIds.begin(), stopIds.end()});
    addTripsToRouterBuilder(subRoute);
  }
} // converter