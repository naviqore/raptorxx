//
// Created by MichaelBrunner on 15/07/2024.
//

#include "GtfsCsvReaderStrategyFactory.h"

#include "strategies/csv_reader/GtfsAgencyReaderCsv.h"
#include "strategies/csv_reader/GtfsCalendarDateReaderCsv.h"
#include "strategies/csv_reader/GtfsCalendarReaderCsv.h"
#include "strategies/csv_reader/GtfsRouteReaderCsv.h"
#include "strategies/csv_reader/GtfsStopReaderCsv.h"
#include "strategies/csv_reader/GtfsStopTimeReaderCsv.h"
#include "strategies/csv_reader/GtfsTransferReaderCsv.h"
#include "strategies/csv_reader/GtfsTripReaderCsv.h"


namespace schedule::gtfs {
  GtfsCsvReaderStrategyFactory::GtfsCsvReaderStrategyFactory(std::string&& aGtfsFileDirectory)
    : fileDirectory(std::move(aGtfsFileDirectory)) {
    if (this->fileDirectory.empty())
    {
      throw std::invalid_argument("fileDirectory must not be empty");
    }
    init();
  }
  std::function<void(GtfsReader&)>& GtfsCsvReaderStrategyFactory::getStrategy(GtfsStrategyType const aType) {
    return strategies[aType];
  }
  void GtfsCsvReaderStrategyFactory::init() {
    setUpFileNameMap();

    strategies[GtfsStrategyType::AGENCY] = GtfsAgencyReaderCsv(fileNameMap[utils::GTFS_FILE_TYPE::AGENCY]);
    strategies[GtfsStrategyType::CALENDAR] = GtfsCalendarReaderCsv(fileNameMap[utils::GTFS_FILE_TYPE::CALENDAR]);
    strategies[GtfsStrategyType::CALENDAR_DATE] = GtfsCalendarDateReaderCsv(fileNameMap[utils::GTFS_FILE_TYPE::CALENDAR_DATES]);
    strategies[GtfsStrategyType::ROUTE] = GtfsRouteReaderCsv(fileNameMap[utils::GTFS_FILE_TYPE::ROUTES]);
    strategies[GtfsStrategyType::STOP] = GtfsStopReaderCsv(fileNameMap[utils::GTFS_FILE_TYPE::STOP]);
    strategies[GtfsStrategyType::STOP_TIME] = GtfsStopTimeReaderCsv(fileNameMap[utils::GTFS_FILE_TYPE::STOP_TIMES]);
    strategies[GtfsStrategyType::TRANSFER] = GtfsTransferReaderCsv(fileNameMap[utils::GTFS_FILE_TYPE::TRANSFERS]);
    strategies[GtfsStrategyType::TRIP] = GtfsTripReaderCsv(fileNameMap[utils::GTFS_FILE_TYPE::TRIPS]);
  }

  void GtfsCsvReaderStrategyFactory::setUpFileNameMap() {
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
} // gtfs
  // schedule