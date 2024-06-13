

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
    std::cout << "GtfsReaderStrategiesTest::SetUp()" << std::endl;
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
    std::cout << "GtfsReaderStrategiesTest::TearDown()" << std::endl;
    reader.reset(nullptr);
  }
};

TEST_F(GtfsReaderStrategiesTest, testAgencyReader) {
  auto strategy = std::vector<std::function<void(gtfs::GtfsReader&)>>();
  const std::function agencyStrategy = gtfs::GtfsAgencyReader(lFileNameMap.at(gtfs::utils::GTFS_FILE_TYPE::AGENCY));
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

TEST_F(GtfsReaderStrategiesTest, testAgencyReader2) {
  auto strategy = std::vector<std::function<void(gtfs::GtfsReader&)>>();
  const std::function calendarDateStrategy = gtfs::GtfsCalendarDateReader(lFileNameMap.at(gtfs::utils::GTFS_FILE_TYPE::CALENDAR_DATES));
  strategy.push_back(calendarDateStrategy);
  reader = std::make_unique<gtfs::GtfsReader>(std::move(strategy));
  reader->readData();
  const auto& data = reader->getData();
  std::cout << "calendarDates: " << data.get().calendarDates.size() << std::endl;
  ASSERT_TRUE(data.get().calendarDates.empty() == false);

  std::ranges::for_each(data.get().calendarDates, [](const gtfs::CalendarDate& calendarDate) {
    ASSERT_TRUE(calendarDate.serviceId.empty() == false);
    ASSERT_TRUE(calendarDate.date.day().ok());
    ASSERT_TRUE(calendarDate.date.month().ok());
    ASSERT_TRUE(calendarDate.date.year().ok());
    ASSERT_TRUE(calendarDate.exceptionType == gtfs::CalendarDate::SERVICE_ADDED || calendarDate.exceptionType == gtfs::CalendarDate::SERVICE_REMOVED);
  });
}

TEST_F(GtfsReaderStrategiesTest, testRelationManager) {

  auto strategy = std::vector<std::function<void(gtfs::GtfsReader&)>>();
  const std::function stopTimesStrategy = gtfs::GtfsStopTimeReader(lFileNameMap.at(gtfs::utils::GTFS_FILE_TYPE::STOP_TIMES));
  strategy.push_back(stopTimesStrategy);

  const std::function tripsStrategy = gtfs::GtfsTripReader(lFileNameMap.at(gtfs::utils::GTFS_FILE_TYPE::TRIPS));
  strategy.push_back(tripsStrategy);

  reader = std::make_unique<gtfs::GtfsReader>(std::move(strategy));
  reader->readData();

  auto relationManager = gtfs::RelationManager(reader->getData().get());
  relationManager.createRelations();
}



class MyTestSuite : public ::testing::TestWithParam<int>
{
protected:
  // You can put setup code here if needed
};

TEST_P(MyTestSuite, TestName) {
  int param = GetParam();
  // Your test code here, using param
  EXPECT_EQ(param, 1);
}

INSTANTIATE_TEST_SUITE_P(
  MyInstantiation, MyTestSuite,
  ::testing::Values(1, 2, 3),
  [](const testing::TestParamInfo<MyTestSuite::ParamType>& info) {
    // Generate a name for each test case
    std::cout << "info.param: " << info.param << std::endl;
    std::string name = "TestWithParam_" + std::to_string(info.param);
    return name;
  });