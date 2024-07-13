//
// Created by MichaelBrunner on 12/07/2024.
//


#include "DataReader.h"
#include "LoggerFactory.h"
#include "gtfs/GtfsData.h"
#include "gtfs/GtfsReaderStrategyFactory.h"
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

#include "utils/scopedTimer.h"

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

  std::cout << "Hello World";

  std::map<schedule::gtfs::utils::GTFS_FILE_TYPE, std::string> lFileNameMap;

  auto readerFactory = std::make_unique<schedule::gtfs::GtfsReaderStrategyFactory>(dataDirectoryPath.data());

  const auto agencyStrategy = readerFactory->getStrategy(schedule::gtfs::GtfsReaderStrategyFactory::Type::AGENCY);
  const auto calendarStrategy = readerFactory->getStrategy(schedule::gtfs::GtfsReaderStrategyFactory::Type::CALENDAR);
  const auto calendarDatesStrategy = readerFactory->getStrategy(schedule::gtfs::GtfsReaderStrategyFactory::Type::CALENDAR_DATE);
  const auto routesStrategy = readerFactory->getStrategy(schedule::gtfs::GtfsReaderStrategyFactory::Type::ROUTE);
  const auto stopStrategy = readerFactory->getStrategy(schedule::gtfs::GtfsReaderStrategyFactory::Type::STOP);
  const auto stopTimeStrategy = readerFactory->getStrategy(schedule::gtfs::GtfsReaderStrategyFactory::Type::STOP_TIME);
  const auto transferStrategy = readerFactory->getStrategy(schedule::gtfs::GtfsReaderStrategyFactory::Type::TRANSFER);
  const auto tripStrategy = readerFactory->getStrategy(schedule::gtfs::GtfsReaderStrategyFactory::Type::TRIP);

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

      auto calIt = allCalendars.find(trip.serviceId);
      if (calIt != allCalendars.end())
      {
        const auto& calendar = calIt->second;
        calendars.push_back(calendar);

        auto calDateIt = allCalendarDates.find(calendar.serviceId);
        if (calDateIt != allCalendarDates.end())
        {
          for (const auto& calendarDate : calDateIt->second)
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

        auto stopIt = allStops.find(stopTime.stopId);
        if (stopIt != allStops.end())
        {
          stops.insert(stopIt->second);
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

  std::string routesHeader = "route_id,agency_id,route_short_name,route_long_name,route_type\n";
  std::string calendarHeader = "service_id,monday,tuesday,wednesday,thursday,friday,saturday,sunday,start_date,end_date\n";
  std::string calendarDatesHeader = "service_id,date,exception_type\n";
  std::string stopsHeader = "stop_id,stop_name,stop_lat,stop_lon,location_type,parent_station\n";
  std::string stopTimesHeader = "trip_id,arrival_time,departure_time,stop_id,stop_sequence\n";
  std::string tripsHeader = "route_id,service_id,trip_id,trip_headsign,trip_short_name,direction_id\n";
  std::string transfersHeader = "from_stop_id,to_stop_id,transfer_type,min_transfer_time\n";

  std::string gtfsDirectoryForAgency = dataDirectoryPath + "\\" + agencyName;

  // create a subfolder for the agency
  if (false == std::filesystem::create_directory(gtfsDirectoryForAgency))
  {
    getLogger(Target::CONSOLE, LoggerName::GTFS)->info("Error creating directory: " + dataDirectoryPath + agencyName + " it may already exists");
  }

  // write gtfs files in parallel
  std::vector<std::future<void>> futures;
  // Write routes
  futures.emplace_back(std::async(std::launch::async, [&]() {
    std::ofstream file(gtfsDirectoryForAgency + "\\" + "routes.txt");
    file << routesHeader;
    for (const auto& route : routes)
    {
      file << route.routeId << ","
           << std::quoted(route.agencyId) << ","
           << std::quoted(route.routeShortName) << ","
           << std::quoted(route.routeLongName) << ","
           << std::quoted(std::to_string(static_cast<int>(route.routeType)))
           << "\n";
    }
  }));

  // Write trips
  futures.emplace_back(std::async(std::launch::async, [&]() {
    std::ofstream file(gtfsDirectoryForAgency + "\\" + "trips.txt");
    file << tripsHeader;
    for (const auto& trip : trips)
    {
      file << std::quoted(trip.routeId) << ","
           << std::quoted(trip.serviceId) << ","
           << std::quoted(trip.tripId) << ","
           << std::quoted("")
           << ","
           << std::quoted("")
           << ","
           << std::quoted("")
           << "\n";
    }
  }));

  // Write stopTimes
  futures.emplace_back(std::async(std::launch::async, [&]() {
    std::ofstream file(gtfsDirectoryForAgency + "\\" + "stop_times.txt");
    file << stopTimesHeader;
    for (const auto& stopTime : stopTimes)
    {
      file << std::quoted(stopTime.tripId) << ","
           << std::quoted(std::to_string(stopTime.arrivalTime.toSeconds())) << ","
           << std::quoted(std::to_string(stopTime.departureTime)) << ","
           << std::quoted(stopTime.stopId) << ","
           << std::quoted(std::to_string(stopTime.stopSequence)) << "\n";
    }
  }));

  // write stops
  futures.emplace_back(std::async(std::launch::async, [&]() {
    std::ofstream file(gtfsDirectoryForAgency + "\\" + "stops.txt");
    file << stopsHeader;
    for (const auto& stop : stops)
    {
      file << std::quoted(stop.stopId) << ","
           << std::quoted(stop.stopName) << ","
           << std::quoted(std::to_string(stop.stopPoint.getFirstCoordinate())) << ","
           << std::quoted(std::to_string(stop.stopPoint.getSecondCoordinate())) << ","
           << std::quoted("") << ","
           << std::quoted(stop.parentStation) << "\n";
    }
  }));

  auto formatDate = [](const std::chrono::year_month_day& ymd) {
    unsigned year = static_cast<int>(ymd.year());
    unsigned month = static_cast<unsigned>(ymd.month());
    unsigned day = static_cast<unsigned>(ymd.day());

    std::ostringstream oss;
    oss << std::setw(4) << std::setfill('0') << year
        << std::setw(2) << std::setfill('0') << month
        << std::setw(2) << std::setfill('0') << day;

    return oss.str();
  };

  // Write calendar
  futures.emplace_back(std::async(std::launch::async, [&]() {
    std::ofstream file(gtfsDirectoryForAgency + "\\" + "calendar.txt");
    file << calendarHeader;
    for (const auto& calendar : calendars)
    {
      file << std::quoted(calendar.serviceId)
           << ","
           << std::quoted(std::to_string(calendar.weekdayService.at(std::chrono::Monday))) << ","
           << std::quoted(std::to_string(calendar.weekdayService.at(std::chrono::Tuesday))) << ","
           << std::quoted(std::to_string(calendar.weekdayService.at(std::chrono::Wednesday))) << ","
           << std::quoted(std::to_string(calendar.weekdayService.at(std::chrono::Thursday))) << ","
           << std::quoted(std::to_string(calendar.weekdayService.at(std::chrono::Friday))) << ","
           << std::quoted(std::to_string(calendar.weekdayService.at(std::chrono::Saturday))) << ","
           << std::quoted(std::to_string(calendar.weekdayService.at(std::chrono::Sunday))) << ","
           << std::quoted(formatDate(calendar.startDate)) << ","
           << std::quoted(formatDate(calendar.endDate)) << "\n";
    }
  }));

  // Write calendar dates
  futures.emplace_back(std::async(std::launch::async, [&]() {
    std::ofstream file(gtfsDirectoryForAgency + "\\" + "calendar_dates.txt");
    file << calendarDatesHeader;
    for (const auto& calendarDate : calendarDates)
    {
      file << std::quoted(calendarDate.serviceId) << ","
           << std::quoted(formatDate(calendarDate.date)) << ","
           << std::quoted(std::to_string(calendarDate.exceptionType)) << "\n";
    }
  }));

  // Write transfer items
  futures.emplace_back(std::async(std::launch::async, [&]() {
    std::ofstream file(gtfsDirectoryForAgency + "\\" + "transfers.txt");
    file << transfersHeader;
    for (const auto& transfer : transferItems)
    {
      file << std::quoted(transfer.fromStopId) << ","
           << std::quoted(transfer.toStopId) << ","
           << std::quoted(std::to_string(transfer.transferType)) << ","
           << std::quoted(std::to_string(transfer.minTransferTime)) << "\n";
    }
  }));

  for (auto& future : futures)
  {
    future.get();
  }

  getLogger(Target::CONSOLE, LoggerName::GTFS)->info("GTFS files written successfully");


  return EXIT_SUCCESS;
}
