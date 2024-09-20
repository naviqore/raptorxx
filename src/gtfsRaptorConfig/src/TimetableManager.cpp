//
// Created by MichaelBrunner on 13/06/2024.
//

#include "TimetableManager.h"

#include <algorithm>
#include <iostream>
#include <ranges>
#include <utility>


namespace converter {

  TimetableManager::TimetableManager(schedule::gtfs::GtfsData&& data, const raptor::utils::LocalDateTime& localDateTime)
    : data(std::make_unique<schedule::gtfs::GtfsData>(std::move(data)))
    , localDateTime(localDateTime)
  {
    createRelations();
    routePartitioner = std::make_unique<RoutePartitioner>(this->data.get());
  }

  void TimetableManager::createRelations()
  {
    getActiveTrips(this->localDateTime);
    buildTripsToRoutesRelations();
    buildStopTimesToTripsAndRoutesRelations();
    buildStopRelations();
  }

  void TimetableManager::buildTripsToRoutesRelations() const
  {
    std::ranges::for_each(this->activeTrips, [this](const schedule::gtfs::Trip* trip) {
      data->getRoute(trip->routeId)->trips.push_back(trip->tripId);
    });
  }

  void TimetableManager::buildStopTimesToTripsAndRoutesRelations() const
  {
    std::ranges::for_each(data->stopTimes | std::views::values | std::views::join, [this](const std::shared_ptr<schedule::gtfs::StopTime>& stopTime) {
      const auto tripServingStopTime = data->getTrip(stopTime->tripId);
      tripServingStopTime->stopTimes.insert(stopTime.get());
    });
  }

  void TimetableManager::buildStopRelations() const
  {
    std::unordered_map<std::string, std::vector<const schedule::gtfs::Transfer*>> transferMap;
    std::ranges::for_each(data->transfers | std::views::values | std::views::join, [&transferMap](const std::shared_ptr<schedule::gtfs::Transfer>& transfer) {
      transferMap[transfer->fromStopId].push_back(transfer.get());
    });

    std::ranges::for_each(data->stops | std::views::values, [this, &transferMap](const std::shared_ptr<schedule::gtfs::Stop>& stop) {
      for (const auto transfer : transferMap[stop->stopId]) {
        stop->transferItems.push_back(transfer);
      }
    });
  }

  bool TimetableManager::isServiceAvailable(const std::string& serviceId, const raptor::utils::LocalDateTime& localDateTime) const
  {
    const auto& calendarDates = data->calendarDates;
    if (const auto calendarDateIt = calendarDates.find(serviceId);
        calendarDateIt != calendarDates.end()) {
      for (const auto& calendarDate : calendarDateIt->second) {
        if (calendarDate->date == localDateTime.toYearMonthDay()) {
          return calendarDate->exceptionType == schedule::gtfs::CalendarDate::ExceptionType::SERVICE_ADDED;
        }
      }
    }

    if (const auto calendarIt = calendarDates.find(serviceId);
        calendarIt == calendarDates.end()) {
      return false;
    }
    const auto& calendarItem = data->calendars.at(serviceId);
    if (localDateTime.toYearMonthDay() < calendarItem->startDate
        || localDateTime.toYearMonthDay() > calendarItem->endDate) {
      return false;
    }
    const auto sysDays = std::chrono::sys_days{localDateTime.toYearMonthDay()}.time_since_epoch().count() % 7;
    const auto weekday = std::chrono::weekday{static_cast<unsigned int>(sysDays)};
    return calendarItem->weekdayService.contains(weekday);
  }

  std::unordered_set<schedule::gtfs::Route*> TimetableManager::getRoutesForActiveTrips()
  {
    if (activeTrips.empty()) {
      getActiveTrips(localDateTime);
    }

    std::unordered_set<schedule::gtfs::Route*> routes;
    for (const auto trip : activeTrips) {
      routes.insert(data->getRoute(trip->routeId));
    }

    return routes;
  }

  const schedule::gtfs::GtfsData& TimetableManager::getData() const
  {
    return *data;
  }

  schedule::gtfs::GtfsData& TimetableManager::getData()
  {
    return *data;
  }
  RoutePartitioner& TimetableManager::getRoutePartitioner()
  {
    return *routePartitioner;
  }

  const RoutePartitioner& TimetableManager::getRoutePartitioner() const
  {
    return *routePartitioner;
  }

  std::unordered_set<schedule::gtfs::Route*> TimetableManager::getRoutes()
  {
    if (activeTrips.empty()) {
      getActiveTrips(localDateTime);
    }

    return getRoutesForActiveTrips();

    // #if defined(__cpp_lib_ranges_to_container)
    //     return data->routes | std::views::values | std::ranges::to<std::vector<schedule::gtfs::Route>>();
    // #else
    //     std::vector<schedule::gtfs::Route> routes;
    //     std::ranges::transform(data->routes, std::back_inserter(routes), [](const auto& route) {
    //       return route.second;
    //     });
    //     return routes;
    // #endif
  }

  void TimetableManager::getActiveTrips(const raptor::utils::LocalDateTime& localDateTime)
  {
    const auto servedDates = data->calendarDates
                             | std::views::values
                             | std::views::join
                             | std::views::filter([this, &localDateTime](const std::shared_ptr<schedule::gtfs::CalendarDate>& date) {
                                 return this->isServiceAvailable(date->serviceId, localDateTime);
                               })
                             | std::views::transform([](const std::shared_ptr<schedule::gtfs::CalendarDate>& date) {
                                 return date->serviceId;
                               })
                             | std::ranges::to<std::set<std::string>>();

    std::ranges::transform(data->trips
                             | std::views::values
                             | std::views::filter([&servedDates, this](const std::shared_ptr<schedule::gtfs::Trip>& trip) {
                                 return servedDates.contains(trip->serviceId);
                               }),
                           std::back_inserter(this->activeTrips),
                           [](const std::shared_ptr<schedule::gtfs::Trip>& trip) {
                             return trip.get();
                           });
  }

} // gtfs
