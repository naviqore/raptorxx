//
// Created by MichaelBrunner on 12/07/2024.
//


#include "DataReader.h"
#include "GtfsReaderStrategyFactory.h"
#include "LoggerFactory.h"
#include "GtfsData.h"
#include "gtfs/GtfsTxtReaderStrategyFactory.h"
#include "TimetableManager.h"
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
      getConsoleLogger(LoggerName::GTFS)->error(std::format("argv[{}] = {}", i, argv[i]));
    }
    getConsoleLogger(LoggerName::GTFS)->error("Usage: " + std::string(argv[0]) + " <data_directory_path> <agency_name>");
    return EXIT_FAILURE;
  }

  std::string dataDirectoryPath = argv[1];
  // Ensure the data directory path ends with a "/"
  if (!dataDirectoryPath.empty() && dataDirectoryPath.back() != '/')
  {
    dataDirectoryPath += '/';
  }
  std::string agencyName = argv[2];

  auto dataDirectoryToWriteFilesTo = dataDirectoryPath;

  std::map<schedule::gtfs::utils::GTFS_FILE_TYPE, std::string> lFileNameMap;

  auto readerFactory = schedule::gtfs::createGtfsReaderStrategyFactory(schedule::gtfs::ReaderType::CSV_PARALLEL, std::move(dataDirectoryPath));

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
  auto relationManager = schedule::gtfs::TimetableManager(std::move(reader->getData().get()));

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
      const auto& currentTrip = data.trips.at(trip);
      trips.push_back(currentTrip);

      auto calendarIterator = allCalendars.find(currentTrip.serviceId);
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
        getLogger(Target::CONSOLE, LoggerName::GTFS)->warn("No calendar found for service id: " + currentTrip.serviceId);
      }

      auto stopTimesForTrip = data.stopTimes | std::views::values | std::views::join | std::views::filter([&trip](const auto& stopTime) {
                                return stopTime.tripId == trip;
                              });

      for (const auto& stopTime : stopTimesForTrip)
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

  std::vector<std::string> routesHeader = {"route_id", "agency_id", "route_short_name", "route_long_name", "route_desc", "route_type"};
  std::vector<std::string> calendarHeader = {"service_id", "monday", "tuesday", "wednesday", "thursday", "friday", "saturday", "sunday", "start_date", "end_date"};
  std::vector<std::string> calendarDatesHeader = {"service_id", "date", "exception_type"};
  std::vector<std::string> stopsHeader = {"stop_id", "stop_name", "stop_lat", "stop_lon", "location_type", "parent_station"};
  std::vector<std::string> stopTimesHeader = {"trip_id", "arrival_time", "departure_time", "stop_id", "stop_sequence", "pickup_type", "drop_off_type"};
  std::vector<std::string> tripsHeader = {"route_id", "service_id", "trip_id", "trip_headsign", "trip_short_name", "direction_id", "block_id"};
  std::vector<std::string> transfersHeader = {"from_stop_id", "to_stop_id", "transfer_type", "min_transfer_time"};

  std::string gtfsDirectoryForDataSubset = dataDirectoryToWriteFilesTo + "\\" + agencyName;

  // create a subfolder for the agency
  if (false == std::filesystem::create_directory(gtfsDirectoryForDataSubset))
  {
    getLogger(Target::CONSOLE, LoggerName::GTFS)->info("Could not create directory: " + gtfsDirectoryForDataSubset + " ,it may already exists");
  }

  // write gtfs files in parallel
  std::vector<std::future<void>> futures;
  // Write routes
  futures.emplace_back(std::async(std::launch::async, [&]() {
    std::ofstream file(gtfsDirectoryForDataSubset + "\\" + "routes.txt", std::ios::binary);
    csv2::Writer<csv2::delimiter<','>> writer(file);
    writer.write_row(routesHeader);
    for (const auto& route : routes)
    {
      std::vector<std::string> row;
      row.push_back(quote(route.routeId));
      row.push_back(quote(route.agencyId));
      row.push_back(quote(route.routeShortName));
      row.push_back(quote(route.routeLongName));
      row.push_back(quote(std::string{})); // TODO use this field for future use
      row.push_back(quote(static_cast<int>(route.routeType)));

      writer.write_row(row);
    }
    file.close();
  }));

  // Write trips
  futures.emplace_back(std::async(std::launch::async, [&]() {
    std::ofstream file(gtfsDirectoryForDataSubset + "\\" + "trips.txt", std::ios::binary);
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
      row.push_back(quote(""));
      writer.write_row(row);
    }
    file.close();
  }));

  // Write stopTimes
  futures.emplace_back(std::async(std::launch::async, [&]() {
    std::ofstream file(gtfsDirectoryForDataSubset + "\\" + "stop_times.txt", std::ios::binary);
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
      row.push_back(quote(""));
      row.push_back(quote(""));

      writer.write_row(row);
    }
    file.close();
  }));

  // write stops
  futures.emplace_back(std::async(std::launch::async, [&]() {
    std::ofstream file(gtfsDirectoryForDataSubset + "\\" + "stops.txt", std::ios::binary);
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
    file.close();
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
    std::ofstream file(gtfsDirectoryForDataSubset + "\\" + "calendar.txt", std::ios::binary);
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
    file.close();
  }));

  // Write calendar dates
  futures.emplace_back(std::async(std::launch::async, [&]() {
    std::ofstream file(gtfsDirectoryForDataSubset + "\\" + "calendar_dates.txt", std::ios::binary);
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
    file.close();
  }));

  // Write transfer items
  futures.emplace_back(std::async(std::launch::async, [&]() {
    std::ofstream file(gtfsDirectoryForDataSubset + "\\" + "transfers.txt", std::ios::binary);
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
    file.close();
  }));

  for (auto& future : futures)
  {
    future.get();
  }

  getConsoleLogger(LoggerName::GTFS)->info("GTFS files written successfully");

  getConsoleLogger(LoggerName::GTFS)->info("GTFS files written to directory: " + gtfsDirectoryForDataSubset);



  return EXIT_SUCCESS;
}
