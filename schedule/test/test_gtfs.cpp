

#include "DataReader.h"
#include "gtfs/GtfsReader.h"
#include "gtfs/GtfsData.h"
#include "LoggingPool.h"
#include "gtfs/GtfsData.h"
#include "gtfs/RelationManager.h"
#include "gtfs/strategies/GtfsAgencyReader.h"
#include "gtfs/strategies/GtfsCalendarDateReader.h"
#include "gtfs/strategies/GtfsCalendarReader.h"
#include "gtfs/strategies/GtfsRouteReader.h"
#include "gtfs/strategies/GtfsStopReader.h"
#include "gtfs/strategies/GtfsStopTimeReader.h"
#include "gtfs/strategies/GtfsTransferReader.h"
#include "gtfs/strategies/GtfsTripReader.h"
#include "utils/utils.h"
#include "src/model/CalendarDate.h"
#include "utils/DataContainer.h"

#include <algorithm>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <map>
#include <memory>
#include <functional>
#include <string>
#include <iostream>


/////////////// TESTS ///////////////
/////////////////////////////////////
/// RUN TESTS IN DEBUG MODE ONLY ////
/////////////////////////////////////
/////////////////////////////////////


class GtfsReaderStrategiesTest : public ::testing::Test
{
protected:
  std::unique_ptr<schedule::DataReader<schedule::DataContainer<gtfs::GtfsData>>> reader;

  const std::string basePath = TEST_DATA_DIR;

  std::map<gtfs::utils::GTFS_FILE_TYPE, std::string> lFileNameMap;

  void SetUp() override {
    std::cout << "GtfsReaderStrategiesTest::SetUp()" << '\n';
    const std::string agencyFile = basePath + "agency.txt";
    const std::string calendarFile = basePath + "calendar.txt";
    const std::string calendarDatesFile = basePath + "calendar_dates.txt";
    const std::string routesFile = basePath + "routes.txt";
    const std::string stopFile = basePath + "stops.txt";
    const std::string stopTimeFile = basePath + "stop_times.txt";
    const std::string TransferFile = basePath + "transfers.txt";
    const std::string TripFile = basePath + "trips.txt";

    lFileNameMap = {
      {gtfs::utils::GTFS_FILE_TYPE::AGENCY, agencyFile},
      {gtfs::utils::GTFS_FILE_TYPE::CALENDAR, calendarFile},
      {gtfs::utils::GTFS_FILE_TYPE::CALENDAR_DATES, calendarDatesFile},
      {gtfs::utils::GTFS_FILE_TYPE::ROUTES, routesFile},
      {gtfs::utils::GTFS_FILE_TYPE::STOP, stopFile},
      {gtfs::utils::GTFS_FILE_TYPE::STOP_TIMES, stopTimeFile},
      {gtfs::utils::GTFS_FILE_TYPE::TRANSFERS, TransferFile},
      {gtfs::utils::GTFS_FILE_TYPE::TRIPS, TripFile}};
  }

  void TearDown() override {
    std::cout << "GtfsReaderStrategiesTest::TearDown()" << '\n';
    reader.reset(nullptr);
  }
};

TEST_F(GtfsReaderStrategiesTest, testAgencyReader) {
  auto strategy = std::vector<std::function<void(gtfs::GtfsReader&)>>();
  const std::function<void(gtfs::GtfsReader&)> agencyStrategy = gtfs::GtfsAgencyReader(lFileNameMap.at(gtfs::utils::GTFS_FILE_TYPE::AGENCY));
  strategy.push_back(agencyStrategy);
  reader = std::make_unique<gtfs::GtfsReader>(std::move(strategy));
  reader->readData();
  const gtfs::GtfsData& data = reader->getData().get();
  std::cout << "agencies: " << data.agencies.size() << std::endl;
  ASSERT_TRUE(data.agencies.empty() == false);
  ASSERT_EQ(data.agencies[0].name, "Schweizerische Bundesbahnen SBB");
  ASSERT_EQ(data.agencies[0].agencyId, "11");
  ASSERT_EQ(data.agencies[0].timezone, "Europe/Berlin");
}

TEST_F(GtfsReaderStrategiesTest, testCalendarDateReader) {
  auto strategy = std::vector<std::function<void(gtfs::GtfsReader&)>>();
  const std::function<void(gtfs::GtfsReader&)> calendarDateStrategy = gtfs::GtfsCalendarDateReader(lFileNameMap.at(gtfs::utils::GTFS_FILE_TYPE::CALENDAR_DATES));
  strategy.push_back(calendarDateStrategy);
  reader = std::make_unique<gtfs::GtfsReader>(std::move(strategy));
  reader->readData();
  const auto& data = reader->getData();
  std::cout << "calendarDates: " << data.get().calendarDates.size() << '\n';
  ASSERT_TRUE(data.get().calendarDates.empty() == false);

  std::ranges::for_each(data.get().calendarDates, [](const gtfs::CalendarDate& calendarDate) {
    ASSERT_TRUE(calendarDate.serviceId.empty() == false);
    ASSERT_TRUE(calendarDate.date.day().ok());
    ASSERT_TRUE(calendarDate.date.month().ok());
    ASSERT_TRUE(calendarDate.date.year().ok());
    ASSERT_TRUE(calendarDate.exceptionType == gtfs::CalendarDate::SERVICE_ADDED || calendarDate.exceptionType == gtfs::CalendarDate::SERVICE_REMOVED);
  });

  std::string date_str = "20231225";
  int year = std::stoi(date_str.substr(0, 4));
  int month = std::stoi(date_str.substr(4, 2));
  int day = std::stoi(date_str.substr(6, 2));

  std::chrono::year_month_day date = std::chrono::year{year} / month / day;

  ASSERT_EQ(data.get().calendarDates[0].serviceId, "TA+00060");
  ASSERT_EQ(data.get().calendarDates[0].date, date);
  ASSERT_TRUE(data.get().calendarDates[0].exceptionType == gtfs::CalendarDate::SERVICE_ADDED);
}

TEST_F(GtfsReaderStrategiesTest, testCalendarReader) {
  auto strategy = std::vector<std::function<void(gtfs::GtfsReader&)>>();
  const std::function<void(gtfs::GtfsReader&)> agencyStrategy = gtfs::GtfsCalendarReader(lFileNameMap.at(gtfs::utils::GTFS_FILE_TYPE::CALENDAR));
  strategy.push_back(agencyStrategy);
  reader = std::make_unique<gtfs::GtfsReader>(std::move(strategy));
  reader->readData();
  const gtfs::GtfsData& data = reader->getData().get();
  ASSERT_TRUE(data.calendars.empty() == false);
  ASSERT_EQ(data.calendars[0].serviceId, "TA");
  ASSERT_EQ(data.calendars[0].weekdayService.at(std::chrono::Monday), 1);
  ASSERT_EQ(static_cast<unsigned>(data.calendars[0].startDate.day()), 10);
  ASSERT_EQ(static_cast<unsigned>(data.calendars[0].startDate.month()), 12);
  ASSERT_EQ(static_cast<int>(data.calendars[0].startDate.year()), 2023);
}

TEST_F(GtfsReaderStrategiesTest, testStopReader) {
  auto strategy = std::vector<std::function<void(gtfs::GtfsReader&)>>();
  const std::function<void(gtfs::GtfsReader&)> agencyStrategy = gtfs::GtfsStopReader(lFileNameMap.at(gtfs::utils::GTFS_FILE_TYPE::STOP));
  strategy.push_back(agencyStrategy);
  reader = std::make_unique<gtfs::GtfsReader>(std::move(strategy));
  reader->readData();
  const gtfs::GtfsData& data = reader->getData().get();
  ASSERT_TRUE(data.stops.empty() == false);
  ASSERT_EQ(data.stops[5].stopId, "1100013");
  ASSERT_EQ(data.stops[5].stopName, "Mambach, Silbersau");
  ASSERT_NEAR(data.stops[5].stopPoint.getFirstCoordinate(), 47.7395192233867, 1e-6);
  ASSERT_NEAR(data.stops[5].stopPoint.getSecondCoordinate(), 7.88223152899259, 1e-6);
  ASSERT_EQ(data.stops[5].parentStation, "Parent1100013");
}

TEST_F(GtfsReaderStrategiesTest, testStopTimeReader) {
  auto strategy = std::vector<std::function<void(gtfs::GtfsReader&)>>();
  const std::function<void(gtfs::GtfsReader&)> agencyStrategy = gtfs::GtfsStopTimeReader(lFileNameMap.at(gtfs::utils::GTFS_FILE_TYPE::STOP_TIMES));
  strategy.push_back(agencyStrategy);
  reader = std::make_unique<gtfs::GtfsReader>(std::move(strategy));
  reader->readData();
  const gtfs::GtfsData& data = reader->getData().get();
  ASSERT_TRUE(data.stopTimes.empty() == false);
  ASSERT_EQ(data.stopTimes[0].tripId, "1.TA.91-10-A-j24-1.1.H");
  ASSERT_EQ(data.stopTimes[0].arrivalTime, gtfs::utils::ServiceDayTime(18, 27, 0));
  ASSERT_EQ(data.stopTimes[0].departureTime, gtfs::utils::ServiceDayTime(18, 27, 0));
  ASSERT_EQ(data.stopTimes[0].stopId, "8503054:0:1");
  ASSERT_EQ(data.stopTimes[0].stopSequence, 1);
}

TEST_F(GtfsReaderStrategiesTest, testTransferReader) {
  auto strategy = std::vector<std::function<void(gtfs::GtfsReader&)>>();
  const std::function<void(gtfs::GtfsReader&)> agencyStrategy = gtfs::GtfsTransferReader(lFileNameMap.at(gtfs::utils::GTFS_FILE_TYPE::TRANSFERS));
  strategy.push_back(agencyStrategy);
  reader = std::make_unique<gtfs::GtfsReader>(std::move(strategy));
  reader->readData();
  const gtfs::GtfsData& data = reader->getData().get();
  ASSERT_TRUE(data.transfers.empty() == false);
  ASSERT_EQ(data.transfers[0].fromStopId, "1100079");
  ASSERT_EQ(data.transfers[0].toStopId, "8014441");
  ASSERT_EQ(data.transfers[0].transferType, 2);
  ASSERT_EQ(data.transfers[0].minTransferTime, 240);
}

TEST_F(GtfsReaderStrategiesTest, testTripReader) {
  auto strategy = std::vector<std::function<void(gtfs::GtfsReader&)>>();
  const std::function<void(gtfs::GtfsReader&)> agencyStrategy = gtfs::GtfsTripReader(lFileNameMap.at(gtfs::utils::GTFS_FILE_TYPE::TRIPS));
  strategy.push_back(agencyStrategy);
  reader = std::make_unique<gtfs::GtfsReader>(std::move(strategy));
  reader->readData();
  const gtfs::GtfsData& data = reader->getData().get();
  ASSERT_TRUE(data.trips.empty() == false);
  ASSERT_EQ(data.trips[0].routeId, "91-10-A-j24-1");
  ASSERT_EQ(data.trips[0].serviceId, "TA+p60e0");
  ASSERT_EQ(data.trips[0].tripId, "1.TA.91-10-A-j24-1.1.H");
}

TEST_F(GtfsReaderStrategiesTest, testRelationManager) {

  auto strategy = std::vector<std::function<void(gtfs::GtfsReader&)>>();
  const std::function<void(gtfs::GtfsReader&)> stopTimesStrategy = gtfs::GtfsStopTimeReader(lFileNameMap.at(gtfs::utils::GTFS_FILE_TYPE::STOP_TIMES));
  const std::function<void(gtfs::GtfsReader&)> tripsStrategy = gtfs::GtfsTripReader(lFileNameMap.at(gtfs::utils::GTFS_FILE_TYPE::TRIPS));

  strategy.push_back(stopTimesStrategy);
  strategy.push_back(tripsStrategy);

  reader = std::make_unique<gtfs::GtfsReader>(std::move(strategy));
  reader->readData();

  auto relationManager = gtfs::RelationManager(reader->getData().get());
  relationManager.createRelations();
}
