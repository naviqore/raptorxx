//
// Created by MichaelBrunner on 23/06/2024.
//

#include "GtfsTxtReaderStrategyFactory.h"

#include <utility>

#include "strategies/txt_reader/GtfsAgencyReader.h"
#include "strategies/txt_reader/GtfsCalendarDateReader.h"
#include "strategies/txt_reader/GtfsCalendarReader.h"
#include "strategies/txt_reader/GtfsRouteReader.h"
#include "strategies/txt_reader/GtfsStopReader.h"
#include "strategies/txt_reader/GtfsStopTimeReader.h"
#include "strategies/txt_reader/GtfsTransferReader.h"
#include "strategies/txt_reader/GtfsTripReader.h"


namespace schedule::gtfs {

  void GtfsTxtReaderStrategyFactory::init() {
    setUpFileNameMap();

    strategies[GtfsStrategyType::AGENCY] = GtfsAgencyReader(fileNameMap[utils::GTFS_FILE_TYPE::AGENCY]);
    strategies[GtfsStrategyType::CALENDAR] = GtfsCalendarReader(fileNameMap[utils::GTFS_FILE_TYPE::CALENDAR]);
    strategies[GtfsStrategyType::CALENDAR_DATE] = GtfsCalendarDateReader(fileNameMap[utils::GTFS_FILE_TYPE::CALENDAR_DATES]);
    strategies[GtfsStrategyType::ROUTE] = GtfsRouteReader(fileNameMap[utils::GTFS_FILE_TYPE::ROUTES]);
    strategies[GtfsStrategyType::STOP] = GtfsStopReader(fileNameMap[utils::GTFS_FILE_TYPE::STOP]);
    strategies[GtfsStrategyType::STOP_TIME] = GtfsStopTimeReader(fileNameMap[utils::GTFS_FILE_TYPE::STOP_TIMES]);
    strategies[GtfsStrategyType::TRANSFER] = GtfsTransferReader(fileNameMap[utils::GTFS_FILE_TYPE::TRANSFERS]);
    strategies[GtfsStrategyType::TRIP] = GtfsTripReader(fileNameMap[utils::GTFS_FILE_TYPE::TRIPS]);
  }

  void GtfsTxtReaderStrategyFactory::setUpFileNameMap() {
    const std::string agencyFile = fileDirectory + "agency.txt";
    const std::string calendarFile = fileDirectory + "calendar.txt";
    const std::string calendarDatesFile = fileDirectory + "calendar_dates.txt";
    const std::string routesFile = fileDirectory + "routes.txt";
    const std::string stopFile = fileDirectory + "stops.txt";
    const std::string stopTimeFile = fileDirectory + "stop_times.txt";
    const std::string TransferFile = fileDirectory + "transfers.txt";
    const std::string TripFile = fileDirectory + "trips.txt";

    fileNameMap = {
      {utils::GTFS_FILE_TYPE::AGENCY, agencyFile},
      {utils::GTFS_FILE_TYPE::CALENDAR, calendarFile},
      {utils::GTFS_FILE_TYPE::CALENDAR_DATES, calendarDatesFile},
      {utils::GTFS_FILE_TYPE::ROUTES, routesFile},
      {utils::GTFS_FILE_TYPE::STOP, stopFile},
      {utils::GTFS_FILE_TYPE::STOP_TIMES, stopTimeFile},
      {utils::GTFS_FILE_TYPE::TRANSFERS, TransferFile},
      {utils::GTFS_FILE_TYPE::TRIPS, TripFile}};
  }

  GtfsTxtReaderStrategyFactory::GtfsTxtReaderStrategyFactory(std::string&& aGtfsFileDirectory)
    : fileDirectory(std::move(aGtfsFileDirectory)) {
    if (fileDirectory.empty())
    {
      throw std::invalid_argument("fileDirectory must not be empty");
    }
    init();
  }

  std::function<void(GtfsReader&)>& GtfsTxtReaderStrategyFactory::getStrategy(const GtfsStrategyType aType) {
    return strategies[aType];
  }
} // gtfs
  // schedule