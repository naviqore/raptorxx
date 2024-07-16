//
// Created by MichaelBrunner on 12/07/2024.
//


#include "DataReader.h"
#include "GtfsReaderStrategyFactory.h"
#include "LoggerFactory.h"
#include "gtfs/GtfsData.h"
#include "gtfs/GtfsTxtReaderStrategyFactory.h"
#include "gtfs/RelationManager.h"
#include "utils/DataContainer.h"


#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <future>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <string>

#include <csv2/writer.hpp>

#include "utils/scopedTimer.h"

template<typename T>
std::string quote(const T& value) {
  return std::format("\"{}\"", value);
}


int main(int argc, char* argv[]) {

  MEASURE_FUNCTION(std::source_location().file_name());

  if (argc != 3)
  {
    for (int i = 0; i < argc; ++i)
    {
      std::cerr << "argv[" << i << "] = " << argv[i] << '\n';
    }
    std::cerr << "Usage: " << argv[0] << " <data_directory_path> <agency_name>" << '\n';
    return EXIT_FAILURE;
  }

  std::string dataDirectoryPath = argv[1];
  // Ensure the data directory path ends with a "/"
  if (!dataDirectoryPath.empty() && dataDirectoryPath.back() != '/')
  {
    dataDirectoryPath += '/';
  }
  std::string agencyName = argv[2];

  std::map<schedule::gtfs::utils::GTFS_FILE_TYPE, std::string> lFileNameMap;

  auto readerFactory = schedule::gtfs::createGtfsReaderStrategyFactory(schedule::gtfs::ReaderType::CSV, std::move(dataDirectoryPath));

  const auto agencyStrategy = readerFactory->getStrategy(GtfsStrategyType::AGENCY);
  const auto calendarStrategy = readerFactory->getStrategy(GtfsStrategyType::CALENDAR);
  const auto calendarDatesStrategy = readerFactory->getStrategy(GtfsStrategyType::CALENDAR_DATE);
  const auto routesStrategy = readerFactory->getStrategy(GtfsStrategyType::ROUTE);
  const auto stopStrategy = readerFactory->getStrategy(GtfsStrategyType::STOP);
  const auto stopTimeStrategy = readerFactory->getStrategy(GtfsStrategyType::STOP_TIME);
  const auto transferStrategy = readerFactory->getStrategy(GtfsStrategyType::TRANSFER);
  const auto tripStrategy = readerFactory->getStrategy(GtfsStrategyType::TRIP);

  std::vector strategies = {agencyStrategy, calendarStrategy, calendarDatesStrategy, routesStrategy, stopStrategy, stopTimeStrategy, transferStrategy, tripStrategy}; //

  const std::unique_ptr<schedule::DataReader<schedule::DataContainer<schedule::gtfs::GtfsData>>> reader = std::make_unique<schedule::gtfs::GtfsReader>(std::move(strategies));
  reader->readData();
  auto relationManager = schedule::gtfs::RelationManager(std::move(reader->getData().get()));

  const auto& agency = relationManager.getData().agencies.at(agencyName);
  getLogger(Target::CONSOLE, LoggerName::GTFS)->info(agency.name);
  getLogger(Target::CONSOLE, LoggerName::GTFS)->info(agency.agencyId);

  std::vector<schedule::gtfs::Route> routes;
  std::vector<schedule::gtfs::Trip> trips;
  std::vector<schedule::gtfs::StopTime> stopTimes;
  std::unordered_set<schedule::gtfs::Stop, decltype(schedule::gtfs::stopHash), decltype(schedule::gtfs::stopEqual)> stops;
  std::vector<schedule::gtfs::Transfer> transferItems;
  std::vector<schedule::gtfs::Calendar> calendars;
  std::vector<schedule::gtfs::CalendarDate> calendarDates;

  const auto& data = relationManager.getData();
  const auto& allRoutes = data.routes;
  const auto& allCalendars = data.calendars;
  const auto& allCalendarDates = data.calendarDates;
  const auto& allStops = data.stops;
  const auto& allTransfersFrom = data.transferFrom;

  for (const auto& [routeId, route] : allRoutes | std::views::filter([&agency](const auto& routeItem) { return routeItem.second.agencyId == agency.agencyId; }))
  {
    routes.push_back(route);

    for (const auto& trip : route.trips)
    {
      trips.push_back(trip);

      auto calendarIterator = allCalendars.find(trip.serviceId);
      if (calendarIterator != allCalendars.end())
      {
        const auto& calendar = calendarIterator->second;
        calendars.push_back(calendar);

        auto calendarDateIterator = allCalendarDates.find(calendar.serviceId);
        if (calendarDateIterator != allCalendarDates.end())
        {
          for (const auto& calendarDate : calendarDateIterator->second)
          {
            calendarDates.push_back(calendarDate);
          }
        }
        else
        {
          getLogger(Target::CONSOLE, LoggerName::GTFS)->warn("No calendar dates found for service id: " + calendar.serviceId);
        }
      }
      else
      {
        getLogger(Target::CONSOLE, LoggerName::GTFS)->warn("No calendar found for service id: " + trip.serviceId);
      }

      for (const auto& stopTime : trip.stopTimes)
      {
        stopTimes.push_back(stopTime);

        auto stopIterator = allStops.find(stopTime.stopId);
        if (stopIterator != allStops.end())
        {
          stops.insert(stopIterator->second);
        }
        else
        {
          getLogger(Target::CONSOLE, LoggerName::GTFS)->warn("No stop found for stop id: " + stopTime.stopId);
        }

        auto transferRange = allTransfersFrom | std::views::values | std::views::filter([&stopTime](const auto& transfers) {
                               return std::ranges::any_of(transfers, [&stopTime](const auto& transfer) {
                                 return transfer.fromStopId == stopTime.stopId;
                               });
                             });

        for (const auto& transfers : transferRange)
        {
          for (const auto& transfer : transfers)
          {
            if (transfer.fromStopId == stopTime.stopId)
            {
              transferItems.push_back(transfer);
            }
          }
        }
      }
    }
  }

  std::vector<std::string> routesHeader = {"route_id", "agency_id", "route_short_name", "route_long_name", "route_type"};
  std::vector<std::string> calendarHeader = {"service_id", "monday", "tuesday", "wednesday", "thursday", "friday", "saturday", "sunday", "start_date", "end_date"};
  std::vector<std::string> calendarDatesHeader = {"service_id", "date", "exception_type"};
  std::vector<std::string> stopsHeader = {"stop_id", "stop_name", "stop_lat", "stop_lon", "zone_id", "parent_station"};
  std::vector<std::string> stopTimesHeader = {"trip_id", "arrival_time", "departure_time", "stop_id", "stop_sequence"};
  std::vector<std::string> tripsHeader = {"route_id", "service_id", "trip_id", "trip_headsign", "trip_short_name", "direction_id"};
  std::vector<std::string> transfersHeader = {"from_stop_id", "to_stop_id", "transfer_type", "min_transfer_time"};

  std::string gtfsDirectoryForAgency = dataDirectoryPath + "\\" + agencyName;

  // create a subfolder for the agency
  if (false == std::filesystem::create_directory(gtfsDirectoryForAgency))
  {
    getLogger(Target::CONSOLE, LoggerName::GTFS)->info("Error creating directory: " + dataDirectoryPath + agencyName + " ,it may already exists");
  }

  // write gtfs files in parallel
  std::vector<std::future<void>> futures;
  // Write routes
  futures.emplace_back(std::async(std::launch::async, [&]() {
    std::ofstream file(gtfsDirectoryForAgency + "\\" + "routes.txt", std::ios::binary);
    csv2::Writer<csv2::delimiter<','>> writer(file);
    writer.write_row(routesHeader);
    for (const auto& route : routes)
    {
      std::vector<std::string> row;
      row.push_back(quote(route.routeId));
      row.push_back(quote(route.agencyId));
      row.push_back(quote(route.routeShortName));
      row.push_back(quote(route.routeLongName));
      row.push_back(quote(static_cast<int>(route.routeType)));

      writer.write_row(row);
    }
  }));

  // Write trips
  futures.emplace_back(std::async(std::launch::async, [&]() {
    std::ofstream file(gtfsDirectoryForAgency + "\\" + "trips.txt", std::ios::binary);
    csv2::Writer<csv2::delimiter<','>> writer(file);
    writer.write_row(tripsHeader);
    for (const auto& trip : trips)
    {
      std::vector<std::string> row;
      row.push_back(quote(trip.routeId));
      row.push_back(quote(trip.serviceId));
      row.push_back(quote(trip.tripId));
      row.push_back(quote(""));
      row.push_back(quote(""));
      row.push_back(quote(""));
      writer.write_row(row);
    }
  }));

  // Write stopTimes
  futures.emplace_back(std::async(std::launch::async, [&]() {
    std::ofstream file(gtfsDirectoryForAgency + "\\" + "stop_times.txt", std::ios::binary);
    csv2::Writer<csv2::delimiter<','>> writer(file);
    writer.write_row(stopTimesHeader);
    for (const auto& stopTime : stopTimes)
    {
      std::vector<std::string> row;
      row.push_back(quote(stopTime.tripId));
      row.push_back(quote(stopTime.arrivalTime.toString()));
      row.push_back(quote(stopTime.departureTime.toString()));
      row.push_back(quote(stopTime.stopId));
      row.push_back(quote(std::to_string(stopTime.stopSequence)));

      writer.write_row(row);
    }
  }));

  // write stops
  futures.emplace_back(std::async(std::launch::async, [&]() {
    std::ofstream file(gtfsDirectoryForAgency + "\\" + "stops.txt", std::ios::binary);
    csv2::Writer<csv2::delimiter<','>> writer(file);
    writer.write_row(stopsHeader);
    for (const auto& stop : stops)
    {

      std::vector<std::string> row;
      row.push_back(quote(stop.stopId));
      row.push_back(quote(stop.stopName));
      row.push_back(quote(std::to_string(stop.stopPoint.getFirstCoordinate())));
      row.push_back(quote(std::to_string(stop.stopPoint.getSecondCoordinate())));
      row.push_back(quote(""));
      row.push_back(quote(stop.parentStation));

      writer.write_row(row);
    }
  }));

  auto formatDate = [](const std::chrono::year_month_day& ymd) {
    const unsigned year = static_cast<int>(ymd.year());
    const unsigned month = static_cast<unsigned>(ymd.month());
    const unsigned day = static_cast<unsigned>(ymd.day());

    std::ostringstream oss;
    oss << std::setw(4) << std::setfill('0') << year
        << std::setw(2) << std::setfill('0') << month
        << std::setw(2) << std::setfill('0') << day;

    return oss.str();
  };

  // Write calendar
  futures.emplace_back(std::async(std::launch::async, [&]() {
    std::ofstream file(gtfsDirectoryForAgency + "\\" + "calendar.txt", std::ios::binary);
    csv2::Writer<csv2::delimiter<','>> writer(file);
    writer.write_row(calendarHeader);
    for (const auto& calendar : calendars)
    {
      std::vector<std::string> row;
      row.push_back(quote(calendar.serviceId));
      row.push_back(quote(std::to_string(calendar.weekdayService.at(std::chrono::Monday))));
      row.push_back(quote(std::to_string(calendar.weekdayService.at(std::chrono::Tuesday))));
      row.push_back(quote(std::to_string(calendar.weekdayService.at(std::chrono::Wednesday))));
      row.push_back(quote(std::to_string(calendar.weekdayService.at(std::chrono::Thursday))));
      row.push_back(quote(std::to_string(calendar.weekdayService.at(std::chrono::Friday))));
      row.push_back(quote(std::to_string(calendar.weekdayService.at(std::chrono::Saturday))));
      row.push_back(quote(std::to_string(calendar.weekdayService.at(std::chrono::Sunday))));
      row.push_back(quote(formatDate(calendar.startDate)));
      row.push_back(quote(formatDate(calendar.endDate)));

      writer.write_row(row);
    }
  }));

  // Write calendar dates
  futures.emplace_back(std::async(std::launch::async, [&]() {
    std::ofstream file(gtfsDirectoryForAgency + "\\" + "calendar_dates.txt", std::ios::binary);
    csv2::Writer<csv2::delimiter<','>> writer(file);
    writer.write_row(calendarDatesHeader);
    for (const auto& calendarDate : calendarDates)
    {

      std::vector<std::string> row;
      row.push_back(quote(calendarDate.serviceId));
      row.push_back(quote(formatDate(calendarDate.date)));
      row.push_back(quote(std::to_string(calendarDate.exceptionType)));

      writer.write_row(row);
    }
  }));

  // Write transfer items
  futures.emplace_back(std::async(std::launch::async, [&]() {
    std::ofstream file(gtfsDirectoryForAgency + "\\" + "transfers.txt", std::ios::binary);
    csv2::Writer<csv2::delimiter<','>> writer(file);
    writer.write_row(transfersHeader);
    for (const auto& transfer : transferItems)
    {

      std::vector<std::string> row;
      row.push_back(quote(transfer.fromStopId));
      row.push_back(quote(transfer.toStopId));
      row.push_back(quote(std::to_string(transfer.transferType)));
      row.push_back(quote(std::to_string(transfer.minTransferTime)));

      writer.write_row(row);
    }
  }));

  for (auto& future : futures)
  {
    future.get();
  }

  getLogger(Target::CONSOLE, LoggerName::GTFS)->info("GTFS files written successfully");


  return EXIT_SUCCESS;
}
