#include <DataContainer.h>

#include <chrono>
#include <cstdlib>
#include <csv2.hpp>
#include <filesystem>
#include <fstream>
#include <future>
#include <iomanip>
#include <iostream>
#include <ranges>
#include <sstream>
#include <string>
#include <unordered_set>

#include "DataReader.h"
#include "GtfsData.h"
#include "GtfsReader.h"
#include "GtfsReaderStrategyFactory.h"
#include "LoggerFactory.h"
#include "TimetableManager.h"

template<typename T>
std::string quote(const T& value)
{
  return std::format("\"{}\"", value);
}

void logUsageError(const char* programName)
{
  getConsoleLogger(LoggerName::GTFS)
    ->error("Usage: " + std::string(programName) + " <data_directory_path> <agency_name>");
}

std::string ensureTrailingSlash(const std::string& path)
{
  return (!path.empty() && path.back() != '/') ? path + '/' : path;
}

std::unique_ptr<
  schedule::DataReader<schedule::DataContainer<schedule::gtfs::GtfsData>>>
createReader(std::string&& dataDirectoryPath)
{
  const auto readerFactory = schedule::gtfs::createGtfsReaderStrategyFactory(
    schedule::gtfs::ReaderType::CSV_PARALLEL, std::move(dataDirectoryPath));
  std::vector strategies = {
    readerFactory->getStrategy(GtfsStrategyType::AGENCY),
    readerFactory->getStrategy(GtfsStrategyType::CALENDAR),
    readerFactory->getStrategy(GtfsStrategyType::CALENDAR_DATE),
    readerFactory->getStrategy(GtfsStrategyType::ROUTE),
    readerFactory->getStrategy(GtfsStrategyType::STOP),
    readerFactory->getStrategy(GtfsStrategyType::STOP_TIME),
    readerFactory->getStrategy(GtfsStrategyType::TRANSFER),
    readerFactory->getStrategy(GtfsStrategyType::TRIP)};
  return std::make_unique<schedule::gtfs::GtfsReader>(std::move(strategies));
}

namespace {
  inline auto stopHash = [](const schedule::gtfs::Stop* stop) {
    return std::hash<std::string>{}(stop->stopId);
  };

  inline auto stopEqual = [](const schedule::gtfs::Stop* lhs, const schedule::gtfs::Stop* rhs) {
    return lhs->stopId == rhs->stopId;
  };

  inline auto transferHash = [](const schedule::gtfs::Transfer* transfer) {
    const auto h1 = std::hash<std::string>{}(transfer->fromStopId);
    const auto h2 = std::hash<std::string>{}(transfer->toStopId);
    const auto h3 = std::hash<int>{}(static_cast<int>(transfer->transferType));
    const auto h4 = std::hash<int>{}(transfer->minTransferTime);

    return h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3);
  };

  inline auto transferEqual = [](const schedule::gtfs::Transfer* lhs, const schedule::gtfs::Transfer* rhs) {
    return lhs->fromStopId == rhs->fromStopId
           && lhs->toStopId == rhs->toStopId
           && lhs->transferType == rhs->transferType
           && lhs->minTransferTime == rhs->minTransferTime;
  };
}

void writeGtfsFiles(const std::string& gtfsDirectoryForDataSubset,
                    const std::vector<const schedule::gtfs::Route*>& routes,
                    const std::vector<const schedule::gtfs::Trip*>& trips,
                    const std::vector<const schedule::gtfs::StopTime*>& stopTimes,
                    const std::unordered_set<const schedule::gtfs::Stop*,
                                             decltype(::stopHash),
                                             decltype(::stopEqual)>& stops,
                    const std::unordered_set<const schedule::gtfs::Transfer*,
                                             decltype(::transferHash),
                                             decltype(::transferEqual)>& transferItems,
                    const std::vector<const schedule::gtfs::Calendar*>& calendars,
                    const std::vector<const schedule::gtfs::CalendarDate*>& calendarDates)
{
  std::vector<std::future<void>> futures;

  auto writeFile = [&](const std::string& filename, auto writeFunc) {
    futures.emplace_back(std::async(std::launch::async, [&, filename, writeFunc]() {
      std::ofstream file(gtfsDirectoryForDataSubset + "\\" + filename, std::ios::binary);
      csv2::Writer<csv2::delimiter<','>> writer(file);
      writeFunc(writer);
      file.close();
    }));
  };

  writeFile("routes.txt", [&](auto& writer) {
    writer.write_row(std::vector{"route_id", "agency_id", "route_short_name", "route_long_name", "route_desc", "route_type"});
    for (const auto& route : routes) {
      writer.write_row(std::vector{
        quote(route->routeId), quote(route->agencyId), quote(route->routeShortName), quote(route->routeLongName), quote(std::string{}), quote(static_cast<int>(route->routeType))});
    }
  });

  writeFile("trips.txt", [&](auto& writer) {
    writer.write_row(std::vector{"route_id", "service_id", "trip_id", "trip_headsign", "trip_short_name", "direction_id", "block_id"});
    for (const auto& trip : trips) {
      writer.write_row(std::vector{quote(trip->routeId), quote(trip->serviceId), quote(trip->tripId), quote(""), quote(""), quote(""), quote("")});
    }
  });

  writeFile("stop_times.txt", [&](auto& writer) {
    writer.write_row(std::vector{"trip_id", "arrival_time", "departure_time", "stop_id", "stop_sequence", "pickup_type", "drop_off_type"});
    for (const auto& stopTime : stopTimes) {
      writer.write_row(std::vector{
        quote(stopTime->tripId), quote(stopTime->arrivalTime.toString()), quote(stopTime->departureTime.toString()), quote(stopTime->stopId), quote(std::to_string(stopTime->stopSequence)), quote(""), quote("")});
    }
  });

  writeFile("stops.txt", [&](auto& writer) {
    writer.write_row(std::vector{"stop_id", "stop_name", "stop_lat", "stop_lon", "location_type", "parent_station"});
    for (const auto& stop : stops) {
      writer.write_row(std::vector{
        quote(stop->stopId), quote(stop->stopName), quote(std::to_string(stop->stopPoint.getFirstCoordinate())), quote(std::to_string(stop->stopPoint.getSecondCoordinate())), quote(""), quote(stop->parentStation)});
    }
  });

  auto formatDate = [](const std::chrono::year_month_day& ymd) {
    const unsigned year = static_cast<int>(ymd.year());
    const unsigned month = static_cast<unsigned>(ymd.month());
    const unsigned day = static_cast<unsigned>(ymd.day());

    std::ostringstream oss;
    oss << std::setw(4) << std::setfill('0') << year << std::setw(2)
        << std::setfill('0') << month << std::setw(2) << std::setfill('0')
        << day;

    return oss.str();
  };

  writeFile("calendar.txt", [&](auto& writer) {
    writer.write_row(std::vector{"service_id", "monday", "tuesday", "wednesday", "thursday", "friday", "saturday", "sunday", "start_date", "end_date"});
    for (const auto& calendar : calendars) {
      writer.write_row(std::vector{
        quote(calendar->serviceId),
        quote(std::to_string(calendar->weekdayService.at(std::chrono::Monday))),
        quote(std::to_string(calendar->weekdayService.at(std::chrono::Tuesday))),
        quote(std::to_string(calendar->weekdayService.at(std::chrono::Wednesday))),
        quote(std::to_string(calendar->weekdayService.at(std::chrono::Thursday))),
        quote(std::to_string(calendar->weekdayService.at(std::chrono::Friday))),
        quote(std::to_string(calendar->weekdayService.at(std::chrono::Saturday))),
        quote(std::to_string(calendar->weekdayService.at(std::chrono::Sunday))),
        quote(formatDate(calendar->startDate)),
        quote(formatDate(calendar->endDate))});
    }
  });

  writeFile("calendar_dates.txt", [&](auto& writer) {
    writer.write_row(std::vector{"service_id", "date", "exception_type"});
    for (const auto& calendarDate : calendarDates) {
      writer.write_row(std::vector{
        quote(calendarDate->serviceId), quote(formatDate(calendarDate->date)), quote(std::to_string(calendarDate->exceptionType))});
    }
  });

  writeFile("transfers.txt", [&](auto& writer) {
    writer.write_row(std::vector{"from_stop_id", "to_stop_id", "transfer_type", "min_transfer_time"});
    for (const auto& transfer : transferItems) {
      writer.write_row(
        std::vector{quote(transfer->fromStopId), quote(transfer->toStopId), quote(std::to_string(transfer->transferType)), quote(std::to_string(transfer->minTransferTime))});
    }
  });

  for (auto& future : futures) {
    future.get();
  }
}

int main(int argc, char* argv[])
{
  if (argc != 3) {
    logUsageError(argv[0]);
    return EXIT_FAILURE;
  }

  std::string dataDirectoryPath = ensureTrailingSlash(argv[1]);
  const std::string agencyName = argv[2];
  const std::string gtfsDirectoryForDataSubset = dataDirectoryPath + "\\" + agencyName;

  if (!std::filesystem::create_directory(gtfsDirectoryForDataSubset)) {
    getLogger(Target::CONSOLE, LoggerName::GTFS)->info("Could not create directory: " + gtfsDirectoryForDataSubset + " ,it may already exist");
  }

  auto reader = createReader(std::move(dataDirectoryPath));
  reader->readData();

  auto relationManager = converter::TimetableManager(std::move(reader->getData().get()));
  const auto& agency = relationManager.getData().agencies.at(agencyName);
  getConsoleLogger(LoggerName::GTFS)->info(agency.name);
  getConsoleLogger(LoggerName::GTFS)->info(agency.agencyId);

  std::vector<const schedule::gtfs::Route*> routes;
  std::vector<const schedule::gtfs::Trip*> trips;
  std::vector<const schedule::gtfs::StopTime*> stopTimes;
  std::unordered_set<const schedule::gtfs::Stop*, decltype(::stopHash), decltype(::stopEqual)> stops;
  std::unordered_set<const schedule::gtfs::Transfer*, decltype(::transferHash), decltype(::transferEqual)> transferItems;
  std::vector<const schedule::gtfs::Calendar*> calendars;
  std::vector<const schedule::gtfs::CalendarDate*> calendarDates;

  const auto& data = relationManager.getData();
  const auto& allRoutes = data.routes;
  const auto& allCalendars = data.calendars;
  const auto& allCalendarDates = data.calendarDates;
  const auto& allStops = data.stops;
  const auto& allTransfersFrom = data.transfers;
  auto agencyRoutes = allRoutes | std::views::filter([&agency](const auto& routeItem) {
                        return routeItem.second.agencyId == agency.agencyId;
                      });
  for (const auto& route : agencyRoutes | std::views::values) {
    routes.push_back(&route);

    for (const auto& trip : route.trips) {
      getConsoleLogger(LoggerName::GTFS)->info(std::format("Processing Trip id: {}", trip));
      const auto& currentTrip = data.trips.at(trip);
      trips.push_back(&currentTrip);

      if (auto calendarIterator = allCalendars.find(currentTrip.serviceId);
          calendarIterator != allCalendars.end()) {
        const auto& calendar = calendarIterator->second;
        calendars.push_back(&calendar);

        if (auto calendarDateIterator = allCalendarDates.find(calendar.serviceId);
            calendarDateIterator != allCalendarDates.end()) {
          for (const auto& calendarDate : calendarDateIterator->second) {
            calendarDates.push_back(&calendarDate);
          }
        }
        else {
          getConsoleLogger(LoggerName::GTFS)->warn("No calendar dates found for service id: " + calendar.serviceId);
        }
      }
      else {
        getConsoleLogger(LoggerName::GTFS)->warn("No calendar found for service id: " + currentTrip.serviceId);
      }

      for (const auto& stopTime : currentTrip.stopTimes) {
        stopTimes.push_back(stopTime);

        auto stopIterator = allStops.find(stopTime->stopId);
        if (stopIterator != allStops.end()) {
          stops.insert(&stopIterator->second);
        }
        else {
          getConsoleLogger(LoggerName::GTFS)
            ->warn("No stop found for stop id: " + stopTime->stopId);
        }
        std::ranges::for_each(stopIterator->second.transferItems, [&](const schedule::gtfs::Transfer* transferItem) {
          if (const auto transferIterator = allTransfersFrom.find(transferItem->fromStopId);
              transferIterator != allTransfersFrom.end()) {
            std::ranges::for_each(transferIterator->second, [&](const auto& transfer) {
              transferItems.insert(&transfer);
            });
          }
          else {
            getConsoleLogger(LoggerName::GTFS)
              ->warn("No transfer found for stop id: " + transferItem->fromStopId);
          }
        });
      }
    }
  }

  writeGtfsFiles(gtfsDirectoryForDataSubset, routes, trips, stopTimes, stops, transferItems, calendars, calendarDates);

  getConsoleLogger(LoggerName::GTFS)->info("GTFS files written successfully");
  getConsoleLogger(LoggerName::GTFS)->info("GTFS files written to directory: " + gtfsDirectoryForDataSubset);

  return EXIT_SUCCESS;
}