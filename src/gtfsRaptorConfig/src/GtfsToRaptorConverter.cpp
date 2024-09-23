//
// Created by MichaelBrunner on 26/07/2024.
//

#include "GtfsToRaptorConverter.h"

#include "LoggerFactory.h"

namespace converter {

  GtfsToRaptorConverter::GtfsToRaptorConverter(const int defaultSameStopTransferTime, std::unique_ptr<TimetableManager>&& timeTableManager)
    : raptorRouterBuilder(defaultSameStopTransferTime)
    , timetableManager(std::move(timeTableManager))
  {
    routePartitioner = std::make_unique<RoutePartitioner>(&this->timetableManager->getData());
  }

  std::shared_ptr<raptor::RaptorData> GtfsToRaptorConverter::convert()
  {
    auto activeTrips = timetableManager->getData().trips
                       | std::views::values
                       | std::views::filter([](const std::shared_ptr<schedule::gtfs::Trip>& trip) { return trip->isTripActive; });

    for (const auto& trip : activeTrips) {
      processTrip(trip);
    }

    addTransfers();
    return raptorRouterBuilder.build();
  }

  void GtfsToRaptorConverter::processTrip(const std::shared_ptr<schedule::gtfs::Trip>& trip)
  {
    const auto subRoute = routePartitioner->getSubRoute(trip->tripId);
    if (!addedSubRoutes.contains(subRoute)) {
      addSubRoute(subRoute);
    }
    raptorRouterBuilder.addTrip(trip->tripId, subRoute.getSubRouteId());

    const auto& stopTimes = trip->stopTimes;
    std::vector<const schedule::gtfs::StopTime*> stopTimesArray;
    stopTimesArray.reserve(stopTimes.size());
    std::ranges::transform(stopTimes, std::back_inserter(stopTimesArray), [](const auto stopTime) { return stopTime; });

    for (const auto [index, stopTime] : std::views::enumerate(stopTimesArray)) {
      addStopTimesToRouterBuilder(*stopTime, trip->tripId, subRoute.getSubRouteId(), static_cast<int>(index));
    }
  }

  void GtfsToRaptorConverter::addSubRoute(const SubRoute& subRoute)
  {
    const auto stopIds = subRoute.getStopsSequence() | std::views::transform([](const schedule::gtfs::Stop* stop) { return stop->stopId; });

    for (const auto& stopId : stopIds) {
      if (!addedStopIds.contains(stopId)) {
        raptorRouterBuilder.addStop(stopId);
        addedStopIds.insert(stopId);
      }
    }
    raptorRouterBuilder.addRoute(subRoute.getSubRouteId(), std::vector<std::string>{stopIds.begin(), stopIds.end()});
    addedSubRoutes.insert(subRoute);
  }

  RoutePartitioner& GtfsToRaptorConverter::getRoutePartitioner() const
  {
    return *routePartitioner;
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
      for (const auto& stop = timetableManager->getData().stops.at(stopId);
           const auto transfer : stop->transferItems) {
        if (transfer->transferType == schedule::gtfs::Transfer::MINIMUM_TIME && transfer->minTransferTime > 0) {
          try {
            raptorRouterBuilder.addTransfer(transfer->fromStopId, transfer->toStopId, transfer->minTransferTime);
          }
          catch (const std::invalid_argument& e) {

#ifdef LOGGER
            getConsoleLogger(LoggerName::RAPTOR)->error(e.what());
#else
            std::ignore = e;
#endif
          }
        }
      }
    }
  }


} // converter
