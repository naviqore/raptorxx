//
// Created by MichaelBrunner on 15/07/2024.
//

#include "GtfsCsvParserReaderStrategyFactory.h"

#include "strategies/csv_reader_fast/GtfsAgencyReaderCsvParser.h"
#include "strategies/csv_reader_fast/GtfsCalendarDateReaderCsvParser.h"
#include "strategies/csv_reader_fast/GtfsCalendarReaderCsvParser.h"
#include "strategies/csv_reader_fast/GtfsRouteReaderCsvParser.h"
#include "strategies/csv_reader_fast/GtfsStopReaderCsvParser.h"
#include "strategies/csv_reader_fast/GtfsStopTimeReaderCsvParser.h"
#include "strategies/csv_reader_fast/GtfsTransferReaderCsvParser.h"
#include "strategies/csv_reader_fast/GtfsTripReaderCsvParser.h"


namespace schedule::gtfs {
  GtfsCsvParserReaderStrategyFactory::GtfsCsvParserReaderStrategyFactory(std::string&& aGtfsFileDirectory)
    : fileDirectory(std::move(aGtfsFileDirectory)) {
    if (this->fileDirectory.empty())
    {
      throw std::invalid_argument("fileDirectory must not be empty");
    }
    init();
  }
  std::function<void(GtfsReader&)>& GtfsCsvParserReaderStrategyFactory::getStrategy(const GtfsStrategyType aType) {
    return strategies[aType];
  }
  void GtfsCsvParserReaderStrategyFactory::init() {
    setUpFileNameMap();

    strategies[GtfsStrategyType::AGENCY] = GtfsAgencyReaderCsvParser(fileNameMap[utils::GTFS_FILE_TYPE::AGENCY]);
    strategies[GtfsStrategyType::CALENDAR] = GtfsCalendarReaderCsvParser(fileNameMap[utils::GTFS_FILE_TYPE::CALENDAR]);
    strategies[GtfsStrategyType::CALENDAR_DATE] = GtfsCalendarDateReaderCsvParser(fileNameMap[utils::GTFS_FILE_TYPE::CALENDAR_DATES]);
    strategies[GtfsStrategyType::ROUTE] = GtfsRouteReaderCsvParser(fileNameMap[utils::GTFS_FILE_TYPE::ROUTES]);
    strategies[GtfsStrategyType::STOP] = GtfsStopReaderCsvParser(fileNameMap[utils::GTFS_FILE_TYPE::STOP]);
    strategies[GtfsStrategyType::STOP_TIME] = GtfsStopTimeReaderCsvParser(fileNameMap[utils::GTFS_FILE_TYPE::STOP_TIMES]);
    strategies[GtfsStrategyType::TRANSFER] = GtfsTransferReaderCsvParser(fileNameMap[utils::GTFS_FILE_TYPE::TRANSFERS]);
    strategies[GtfsStrategyType::TRIP] = GtfsTripReaderCsvParser(fileNameMap[utils::GTFS_FILE_TYPE::TRIPS]);
  }
  void GtfsCsvParserReaderStrategyFactory::setUpFileNameMap() {
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