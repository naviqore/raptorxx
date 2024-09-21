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

  void TimetableManager::createRelations() const
  {
    markActiveTrips(this->localDateTime);
    buildTripsToRoutesRelations();
    buildStopTimesToTripsAndRoutesRelations();
    buildStopRelations();
  }

  void TimetableManager::buildTripsToRoutesRelations() const
  {
    std::ranges::for_each(data->trips | std::views::values, [this](const std::shared_ptr<schedule::gtfs::Trip>& trip) {
      if (trip->isTripActive) {
        data->getRoute(trip->routeId)->trips.insert(trip->tripId);
      }
    });
  }


  void TimetableManager::buildStopTimesToTripsAndRoutesRelations() const
  {
    std::ranges::for_each(data->stopTimes | std::views::values | std::views::join, [this](const std::shared_ptr<schedule::gtfs::StopTime>& stopTime) {
      if (const auto tripServingStopTime = data->getTrip(stopTime->tripId);
          tripServingStopTime->isTripActive) {
        if (this->localDateTime.getSeconds() < stopTime->arrivalTime.toSeconds()) {
          tripServingStopTime->stopTimes.insert(stopTime.get());
        }
      }
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
    const auto currentDate = localDateTime.toYearMonthDay();

    if (const auto calendarDateIt = calendarDates.find(serviceId);
        calendarDateIt != calendarDates.end()) {
      for (const auto& calendarDate : calendarDateIt->second) {
        if (calendarDate->date == currentDate) {
          return calendarDate->exceptionType == schedule::gtfs::CalendarDate::ExceptionType::SERVICE_ADDED;
        }
      }
    }

    if (const auto calendarIt = data->calendars.find(serviceId);
        calendarIt != data->calendars.end()) {
      return calendarIt->second->isServiceAvailable(localDateTime.toYearMonthDay());
    }

    return false;
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

  void TimetableManager::markActiveTrips(const raptor::utils::LocalDateTime& localDateTime) const
  {
    /////////////////////
    std::vector<std::shared_ptr<schedule::gtfs::Trip>> activeTrips;

    // Filter calendars that have service available on the given date
    auto filteredCalendars = data->calendars | std::views::values
                             | std::views::filter([&localDateTime, this](const std::shared_ptr<schedule::gtfs::Calendar>& calendar) {
                                 return this->isServiceAvailable(calendar->serviceId, localDateTime);
                               });
    std::unordered_set<std::string> serviceIds;
    for (const auto& item : filteredCalendars) {
      serviceIds.insert(item->serviceId);
    }

    // Collect trips from the filtered calendars

    for (const auto& trip : data->trips | std::views::values) {
      if (serviceIds.contains(trip->serviceId)) {
        activeTrips.push_back(trip);
      }
    }
    ////////////////////////


    /* const auto servedDates = data->calendarDates
                             | std::views::values
                             | std::views::join
                             | std::views::filter([this, &localDateTime](const std::shared_ptr<schedule::gtfs::CalendarDate>& date) {
                                 return this->isServiceAvailable(date->serviceId, localDateTime);
                               })
                             | std::views::transform([](const std::shared_ptr<schedule::gtfs::CalendarDate>& date) {
                                 return date->serviceId;
                               })
                             | std::ranges::to<std::set<std::string>>();*/

    for (const auto& trip : std::views::filter(data->trips
                                                 | std::views::values,
                                               [&serviceIds, this](const std::shared_ptr<schedule::gtfs::Trip>& currentTrip) {
                                                 return serviceIds.contains(currentTrip->serviceId);
                                               })) {
      trip->isTripActive = true;
      this->data->getRoute(trip->routeId)->isRouteActive = true;
    }
    std::cout << "availableServices" << serviceIds.size() << std::endl;
    std::cout << "All Trips: " << data->trips.size() << std::endl;
    std::cout << "Active Trips: " << std::ranges::count_if(data->trips | std::views::values, [](const std::shared_ptr<schedule::gtfs::Trip>& trip) {
      return trip->isTripActive;
    }) << std::endl;
  }

} // gtfs
