

#include "DataReader.h"
#include "gtfs/GtfsReader.h"
#include "gtfs/GtfsReaderStrategyFactory.h"
#include "gtfs/RelationManager.h"
#include "gtfs/strategies/txt_reader/GtfsAgencyReader.h"
#include "gtfs/strategies/txt_reader/GtfsCalendarDateReader.h"
#include "gtfs/strategies/txt_reader/GtfsCalendarReader.h"
#include "gtfs/strategies/txt_reader/GtfsRouteReader.h"
#include "gtfs/strategies/txt_reader/GtfsStopReader.h"
#include "gtfs/strategies/txt_reader/GtfsStopTimeReader.h"
#include "gtfs/strategies/txt_reader/GtfsTransferReader.h"
#include "gtfs/strategies/txt_reader/GtfsTripReader.h"
#include "utils/utils.h"
#include "model/CalendarDate.h"
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


class GtfsReaderStrategiesTest : public testing::Test
{
protected:
  std::unique_ptr<schedule::DataReader<schedule::DataContainer<schedule::gtfs::GtfsData>>> reader;

  std::unique_ptr<schedule::gtfs::GtfsReaderStrategyFactory> readerFactory;

  std::string basePath = TEST_DATA_DIR;

  void SetUp() override {
    readerFactory = std::make_unique<schedule::gtfs::GtfsReaderStrategyFactory>(std::move(basePath));
  }

  void TearDown() override {
    std::cout << "GtfsReaderStrategiesTest::TearDown()" << '\n';
    reader.reset(nullptr);
  }
};

TEST_F(GtfsReaderStrategiesTest, testAgencyReader) {
  auto strategy = std::vector<std::function<void(schedule::gtfs::GtfsReader&)>>();
  const std::function<void(schedule::gtfs::GtfsReader&)> strategyReader = readerFactory->getStrategy(schedule::gtfs::GtfsReaderStrategyFactory::Type::AGENCY);
  strategy.push_back(strategyReader);
  reader = std::make_unique<schedule::gtfs::GtfsReader>(std::move(strategy));
  reader->readData();
  const schedule::gtfs::GtfsData& data = reader->getData().get();
  std::cout << "agencies: " << data.agencies.size() << std::endl;

  ASSERT_TRUE(data.agencies.empty() == false);
  EXPECT_STREQ(data.agencies.at("Schweizerische Bundesbahnen SBB").name.c_str(), "Schweizerische Bundesbahnen SBB");
  EXPECT_STREQ(data.agencies.at("Schweizerische Bundesbahnen SBB").agencyId.c_str(), "11");
  EXPECT_STREQ(data.agencies.at("Schweizerische Bundesbahnen SBB").timezone.c_str(), "Europe/Berlin");
}

TEST_F(GtfsReaderStrategiesTest, testCalendarDateReader) {
  auto strategy = std::vector<std::function<void(schedule::gtfs::GtfsReader&)>>();
  const std::function<void(schedule::gtfs::GtfsReader&)> strategyReader = readerFactory->getStrategy(schedule::gtfs::GtfsReaderStrategyFactory::Type::CALENDAR_DATE);
  strategy.push_back(strategyReader);
  reader = std::make_unique<schedule::gtfs::GtfsReader>(std::move(strategy));
  reader->readData();
  const auto& data = reader->getData();
  std::cout << "calendarDates: " << data.get().calendarDates.size() << '\n';
  ASSERT_TRUE(data.get().calendarDates.empty() == false);

  const auto calendarDate = data.get().calendarDates.at("TA+00060");

  std::ranges::for_each(calendarDate, [](const schedule::gtfs::CalendarDate& calendarDate) {
    ASSERT_TRUE(calendarDate.serviceId.empty() == false);
    ASSERT_TRUE(calendarDate.date.day().ok());
    ASSERT_TRUE(calendarDate.date.month().ok());
    ASSERT_TRUE(calendarDate.date.year().ok());
    ASSERT_TRUE(calendarDate.exceptionType == schedule::gtfs::CalendarDate::SERVICE_ADDED || calendarDate.exceptionType == schedule::gtfs::CalendarDate::SERVICE_REMOVED);
  });

  const std::string date_str = "20231216";
  const int year = std::stoi(date_str.substr(0, 4));
  const int month = std::stoi(date_str.substr(4, 2));
  const int day = std::stoi(date_str.substr(6, 2));

  const std::chrono::year_month_day date = std::chrono::year{year} / month / day;

  ASSERT_EQ(calendarDate[0].serviceId, "TA+00060");
  ASSERT_EQ(calendarDate[0].date, date);
  ASSERT_TRUE(calendarDate[0].exceptionType == schedule::gtfs::CalendarDate::SERVICE_REMOVED);
}

TEST_F(GtfsReaderStrategiesTest, testCalendarReader) {
  auto strategy = std::vector<std::function<void(schedule::gtfs::GtfsReader&)>>();
  const std::function<void(schedule::gtfs::GtfsReader&)> strategyReader = readerFactory->getStrategy(schedule::gtfs::GtfsReaderStrategyFactory::Type::CALENDAR);
  strategy.push_back(strategyReader);
  reader = std::make_unique<schedule::gtfs::GtfsReader>(std::move(strategy));
  reader->readData();
  const schedule::gtfs::GtfsData& data = reader->getData().get();
  const auto calender = data.calendars.at("TA");
  ASSERT_TRUE(calender.serviceId.empty() == false);
  ASSERT_EQ(calender.serviceId, "TA");
  ASSERT_EQ(calender.weekdayService.at(std::chrono::Monday), 1);
  ASSERT_EQ(static_cast<unsigned>(calender.startDate.day()), 10);
  ASSERT_EQ(static_cast<unsigned>(calender.startDate.month()), 12);
  ASSERT_EQ(static_cast<int>(calender.startDate.year()), 2023);
}

TEST_F(GtfsReaderStrategiesTest, testStopReader) {
  auto strategy = std::vector<std::function<void(schedule::gtfs::GtfsReader&)>>();
  const std::function<void(schedule::gtfs::GtfsReader&)> strategyReader = readerFactory->getStrategy(schedule::gtfs::GtfsReaderStrategyFactory::Type::STOP);
  strategy.push_back(strategyReader);
  reader = std::make_unique<schedule::gtfs::GtfsReader>(std::move(strategy));
  reader->readData();
  const schedule::gtfs::GtfsData& data = reader->getData().get();
  const auto stop = data.stops.at("1100013");
  ASSERT_TRUE(stop.stopId.empty() == false);
  ASSERT_EQ(stop.stopId, "1100013");
  ASSERT_EQ(stop.stopName, "Mambach, Silbersau");
  ASSERT_NEAR(stop.stopPoint.getFirstCoordinate(), 47.7395192233867, 1e-6);
  ASSERT_NEAR(stop.stopPoint.getSecondCoordinate(), 7.88223152899259, 1e-6);
  ASSERT_EQ(stop.parentStation, "Parent1100013");
}

TEST_F(GtfsReaderStrategiesTest, testStopTimeReader) {
  auto strategy = std::vector<std::function<void(schedule::gtfs::GtfsReader&)>>();
  const std::function<void(schedule::gtfs::GtfsReader&)> strategyReader = readerFactory->getStrategy(schedule::gtfs::GtfsReaderStrategyFactory::Type::STOP_TIME);
  strategy.push_back(strategyReader);
  reader = std::make_unique<schedule::gtfs::GtfsReader>(std::move(strategy));
  reader->readData();
  const schedule::gtfs::GtfsData& data = reader->getData().get();
  const auto stopTimes = data.stopTimes.at("8503054:0:1");

  ASSERT_TRUE(stopTimes[0].stopId.empty() == false);
  ASSERT_EQ(stopTimes[0].tripId, "1.TA.91-10-A-j24-1.1.H");
  ASSERT_EQ(stopTimes[0].arrivalTime, schedule::gtfs::utils::ServiceDayTime(18, 27, 0));
  ASSERT_EQ(stopTimes[0].departureTime, schedule::gtfs::utils::ServiceDayTime(18, 27, 0));
  ASSERT_EQ(stopTimes[0].stopId, "8503054:0:1");
  ASSERT_EQ(stopTimes[0].stopSequence, 1);
}

TEST_F(GtfsReaderStrategiesTest, testTransferReader) {
  auto strategy = std::vector<std::function<void(schedule::gtfs::GtfsReader&)>>();
  const std::function<void(schedule::gtfs::GtfsReader&)> strategyReader = readerFactory->getStrategy(schedule::gtfs::GtfsReaderStrategyFactory::Type::TRANSFER);
  strategy.push_back(strategyReader);
  reader = std::make_unique<schedule::gtfs::GtfsReader>(std::move(strategy));
  reader->readData();
  const schedule::gtfs::GtfsData& data = reader->getData().get();
  const auto transferFrom = data.transferFrom.at("1100079");
  ASSERT_EQ(transferFrom[0].toStopId, "8014441");

  const auto transferTo = data.transferTo.at("8014441");
  ASSERT_EQ(transferFrom[0].fromStopId, "1100079");
}

TEST_F(GtfsReaderStrategiesTest, testTripReader) {
  auto strategy = std::vector<std::function<void(schedule::gtfs::GtfsReader&)>>();
  const std::function<void(schedule::gtfs::GtfsReader&)> strategyReader = readerFactory->getStrategy(schedule::gtfs::GtfsReaderStrategyFactory::Type::TRIP);
  strategy.push_back(strategyReader);
  reader = std::make_unique<schedule::gtfs::GtfsReader>(std::move(strategy));
  reader->readData();

  const schedule::gtfs::GtfsData& data = reader->getData().get();
  const auto trips = data.trips.at("91-10-A-j24-1");
  ASSERT_TRUE(trips.empty() == false);
  ASSERT_EQ(trips[0].routeId, "91-10-A-j24-1");
  ASSERT_EQ(trips[0].serviceId, "TA+p60e0");
  ASSERT_EQ(trips[0].tripId, "1.TA.91-10-A-j24-1.1.H");
}

TEST_F(GtfsReaderStrategiesTest, testrouteReader) {
  auto strategy = std::vector<std::function<void(schedule::gtfs::GtfsReader&)>>();
  const std::function<void(schedule::gtfs::GtfsReader&)> strategyReader = readerFactory->getStrategy(schedule::gtfs::GtfsReaderStrategyFactory::Type::ROUTE);
  strategy.push_back(strategyReader);
  reader = std::make_unique<schedule::gtfs::GtfsReader>(std::move(strategy));
  reader->readData();
  const schedule::gtfs::GtfsData& data = reader->getData().get();
  const auto route = data.routes.at("91-10-E-j24-1");
  ASSERT_TRUE(route.routeId.empty() == false);
  ASSERT_EQ(route.routeId, "91-10-E-j24-1");
  ASSERT_EQ(static_cast<int>(route.routeType), 900);
}

TEST_F(GtfsReaderStrategiesTest, testRelationManager) {

  auto strategy = std::vector<std::function<void(schedule::gtfs::GtfsReader&)>>();
  const std::function<void(schedule::gtfs::GtfsReader&)> stopTimesStrategy = readerFactory->getStrategy(schedule::gtfs::GtfsReaderStrategyFactory::Type::STOP_TIME);
  const std::function<void(schedule::gtfs::GtfsReader&)> tripsStrategy = readerFactory->getStrategy(schedule::gtfs::GtfsReaderStrategyFactory::Type::TRIP);

  strategy.push_back(stopTimesStrategy);
  strategy.push_back(tripsStrategy);

  reader = std::make_unique<schedule::gtfs::GtfsReader>(std::move(strategy));
  reader->readData();

  auto relationManager = schedule::gtfs::RelationManager(std::move(reader->getData().get()));
  relationManager.createRelations();
}
