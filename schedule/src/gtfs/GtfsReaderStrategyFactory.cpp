//
// Created by MichaelBrunner on 23/06/2024.
//

#include "GtfsReaderStrategyFactory.h"

#include <utility>

#include "strategies/GtfsAgencyReader.h"
#include "strategies/GtfsCalendarReader.h"
#include "strategies/GtfsRouteReader.h"
#include "strategies/GtfsStopReader.h"
#include "strategies/GtfsStopTimeReader.h"
#include "strategies/GtfsTransferReader.h"
#include "strategies/GtfsTripReader.h"


namespace schedule::gtfs {
  void GtfsReaderStrategyFactory::init() {
    setUpFileNameMap();

    strategies[AGENCY] = GtfsAgencyReader(lFileNameMap[utils::GTFS_FILE_TYPE::AGENCY]);
    strategies[CALENDAR] = GtfsCalendarReader(lFileNameMap[utils::GTFS_FILE_TYPE::CALENDAR]);
    strategies[CALENDAR_DATE] = GtfsCalendarDateReader(lFileNameMap[utils::GTFS_FILE_TYPE::CALENDAR_DATES]);
    strategies[ROUTE] = GtfsRouteReader(lFileNameMap[utils::GTFS_FILE_TYPE::ROUTES]);
    strategies[STOP] = GtfsStopReader(lFileNameMap[utils::GTFS_FILE_TYPE::STOP]);
    strategies[STOP_TIME] = GtfsStopTimeReader(lFileNameMap[utils::GTFS_FILE_TYPE::STOP_TIMES]);
    strategies[TRANSFER] = GtfsTransferReader(lFileNameMap[utils::GTFS_FILE_TYPE::TRANSFERS]);
    strategies[TRIP] = GtfsTripReader(lFileNameMap[utils::GTFS_FILE_TYPE::TRIPS]);
  }

  void GtfsReaderStrategyFactory::setUpFileNameMap() {
    const std::string agencyFile = fileDirectory + "agency.txt";
    const std::string calendarFile = fileDirectory + "calendar.txt";
    const std::string calendarDatesFile = fileDirectory + "calendar_dates.txt";
    const std::string routesFile = fileDirectory + "routes.txt";
    const std::string stopFile = fileDirectory + "stops.txt";
    const std::string stopTimeFile = fileDirectory + "stop_times.txt";
    const std::string TransferFile = fileDirectory + "transfers.txt";
    const std::string TripFile = fileDirectory + "trips.txt";

    lFileNameMap = {
      {utils::GTFS_FILE_TYPE::AGENCY, agencyFile},
      {utils::GTFS_FILE_TYPE::CALENDAR, calendarFile},
      {utils::GTFS_FILE_TYPE::CALENDAR_DATES, calendarDatesFile},
      {utils::GTFS_FILE_TYPE::ROUTES, routesFile},
      {utils::GTFS_FILE_TYPE::STOP, stopFile},
      {utils::GTFS_FILE_TYPE::STOP_TIMES, stopTimeFile},
      {utils::GTFS_FILE_TYPE::TRANSFERS, TransferFile},
      {utils::GTFS_FILE_TYPE::TRIPS, TripFile}};
  }

  GtfsReaderStrategyFactory::GtfsReaderStrategyFactory(std::string&& aGtfsFileDirectory)
    : fileDirectory(std::move(aGtfsFileDirectory)) {
    if (fileDirectory.empty())
    {
      throw std::invalid_argument("fileDirectory must not be empty");
    }
    init();
  }

  std::function<void(GtfsReader&)>& GtfsReaderStrategyFactory::getStrategy(const Type aType) {
    return strategies[aType];
  }
} // gtfs
  // schedule