

#include "DataReader.h"
#include "GtfsReaderStrategyFactory.h"
#include "include/GtfsReader.h"
#include "model/CalendarDate.h"
#include "DataContainer.h"

#include <algorithm>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <map>
#include <memory>
#include <functional>
#include <string>
#include <iostream>

#include "LoggerFactory.h"
#include <array>
#include <chrono>
#include <fmt/core.h>


class GtfsCsvReaderStrategiesTest : public testing::Test
{
protected:
  std::unique_ptr<schedule::DataReader<schedule::DataContainer<schedule::gtfs::GtfsData>>> reader;

  std::unique_ptr<schedule::gtfs::IGtfsReaderStrategyFactory> readerFactory;

  std::string basePath = TEST_DATA_DIR;

  void SetUp() override {
    readerFactory = schedule::gtfs::createGtfsReaderStrategyFactory(schedule::gtfs::ReaderType::TXT, std::move(basePath));
  }

  void TearDown() override {
    std::cout << "GtfsReaderStrategiesTest::TearDown()" << '\n';
    reader.reset(nullptr);
  }
};

TEST_F(GtfsCsvReaderStrategiesTest, testAgencyReaderTxt) {
  auto strategy = std::vector<std::function<void(schedule::gtfs::GtfsReader&)>>();
  const std::function<void(schedule::gtfs::GtfsReader&)> strategyReader =  readerFactory->getStrategy(GtfsStrategyType::AGENCY);
  strategy.push_back(strategyReader);
  reader = std::make_unique<schedule::gtfs::GtfsReader>(std::move(strategy));
  reader->readData();
  const schedule::gtfs::GtfsData& data = reader->getData().get();
  std::cout << "agencies: " << data.agencies.size() << std::endl;

  ASSERT_TRUE(data.agencies.empty() == false);
  EXPECT_STREQ(data.getAgency("Schweizerische Bundesbahnen SBB")->name.c_str(), "Schweizerische Bundesbahnen SBB");
  EXPECT_STREQ(data.getAgency("Schweizerische Bundesbahnen SBB")->agencyId.c_str(), "11");
  EXPECT_STREQ(data.getAgency("Schweizerische Bundesbahnen SBB")->timezone.c_str(), "Europe/Berlin");
}

TEST_F(GtfsCsvReaderStrategiesTest, testCalendarDateReaderTxt) {
  auto strategy = std::vector<std::function<void(schedule::gtfs::GtfsReader&)>>();
  const std::function<void(schedule::gtfs::GtfsReader&)> strategyReader =  readerFactory->getStrategy(GtfsStrategyType::CALENDAR_DATE);
  strategy.push_back(strategyReader);
  reader = std::make_unique<schedule::gtfs::GtfsReader>(std::move(strategy));
  reader->readData();
  const auto& data = reader->getData();
  std::cout << "calendarDates: " << data.get().calendarDates.size() << '\n';
  ASSERT_TRUE(data.get().calendarDates.empty() == false);

  const auto calendarDate = data.get().getCalendarDates("TA+00060");

  std::ranges::for_each(calendarDate, [](const std::shared_ptr<schedule::gtfs::CalendarDate>& calendarDate) {
    ASSERT_TRUE(calendarDate->serviceId.empty() == false);
    ASSERT_TRUE(calendarDate->date.day().ok());
    ASSERT_TRUE(calendarDate->date.month().ok());
    ASSERT_TRUE(calendarDate->date.year().ok());
    ASSERT_TRUE(calendarDate->exceptionType == schedule::gtfs::CalendarDate::SERVICE_ADDED || calendarDate->exceptionType == schedule::gtfs::CalendarDate::SERVICE_REMOVED);
  });

  const std::string date_str = "20231216";
  const int year = std::stoi(date_str.substr(0, 4));
  const int month = std::stoi(date_str.substr(4, 2));
  const int day = std::stoi(date_str.substr(6, 2));

  const std::chrono::year_month_day date = std::chrono::year{year} / month / day;

  ASSERT_EQ(calendarDate[0]->serviceId, "TA+00060");
  ASSERT_EQ(calendarDate[0]->date, date);
  ASSERT_TRUE(calendarDate[0]->exceptionType == schedule::gtfs::CalendarDate::SERVICE_REMOVED);
}

TEST_F(GtfsCsvReaderStrategiesTest, testCalendarReaderTxt) {
  auto strategy = std::vector<std::function<void(schedule::gtfs::GtfsReader&)>>();
  const std::function<void(schedule::gtfs::GtfsReader&)> strategyReader =  readerFactory->getStrategy(GtfsStrategyType::CALENDAR);
  strategy.push_back(strategyReader);
  reader = std::make_unique<schedule::gtfs::GtfsReader>(std::move(strategy));
  reader->readData();
  const schedule::gtfs::GtfsData& data = reader->getData().get();
  const auto calender = data.getCalendar("TA");
  ASSERT_TRUE(calender->serviceId.empty() == false);
  ASSERT_EQ(calender->serviceId, "TA");
  ASSERT_EQ(calender->weekdayService.at(std::chrono::Monday), 1);
  ASSERT_EQ(static_cast<unsigned>(calender->startDate.day()), 10);
  ASSERT_EQ(static_cast<unsigned>(calender->startDate.month()), 12);
  ASSERT_EQ(static_cast<int>(calender->startDate.year()), 2023);
}

TEST_F(GtfsCsvReaderStrategiesTest, testStopReaderTxt) {
  auto strategy = std::vector<std::function<void(schedule::gtfs::GtfsReader&)>>();
  const std::function<void(schedule::gtfs::GtfsReader&)> strategyReader =  readerFactory->getStrategy(GtfsStrategyType::STOP);
  strategy.push_back(strategyReader);
  reader = std::make_unique<schedule::gtfs::GtfsReader>(std::move(strategy));
  reader->readData();
  const schedule::gtfs::GtfsData& data = reader->getData().get();
  const auto stop = data.getStop("1100013");
  ASSERT_TRUE(stop->stopId.empty() == false);
  ASSERT_EQ(stop->stopId, "1100013");
  ASSERT_EQ(stop->stopName, "Mambach, Silbersau");
  ASSERT_NEAR(stop->stopPoint.getFirstCoordinate(), 47.7395192233867, 1e-6);
  ASSERT_NEAR(stop->stopPoint.getSecondCoordinate(), 7.88223152899259, 1e-6);
  ASSERT_EQ(stop->parentStation, "Parent1100013");
}

TEST_F(GtfsCsvReaderStrategiesTest, testStopTimeReaderTxt) {
  auto strategy = std::vector<std::function<void(schedule::gtfs::GtfsReader&)>>();
  const std::function<void(schedule::gtfs::GtfsReader&)> strategyReader =  readerFactory->getStrategy(GtfsStrategyType::STOP_TIME);
  strategy.push_back(strategyReader);
  reader = std::make_unique<schedule::gtfs::GtfsReader>(std::move(strategy));
  reader->readData();
  const schedule::gtfs::GtfsData& data = reader->getData().get();
  const auto stopTimes = data.getStopTimes("8503054:0:1");

  ASSERT_TRUE(stopTimes[0]->stopId.empty() == false);
  ASSERT_EQ(stopTimes[0]->tripId, "1.TA.91-10-A-j24-1.1.H");
  ASSERT_EQ(stopTimes[0]->arrivalTime, schedule::gtfs::utils::ServiceDayTime(18, 27, 0));
  ASSERT_EQ(stopTimes[0]->departureTime, schedule::gtfs::utils::ServiceDayTime(18, 27, 0));
  ASSERT_EQ(stopTimes[0]->stopId, "8503054:0:1");
  ASSERT_EQ(stopTimes[0]->stopSequence, 1);
}

TEST_F(GtfsCsvReaderStrategiesTest, testTransferReaderTxt) {
  auto strategy = std::vector<std::function<void(schedule::gtfs::GtfsReader&)>>();
  const std::function<void(schedule::gtfs::GtfsReader&)> strategyReader =  readerFactory->getStrategy(GtfsStrategyType::TRANSFER);
  strategy.push_back(strategyReader);
  reader = std::make_unique<schedule::gtfs::GtfsReader>(std::move(strategy));
  reader->readData();
  const schedule::gtfs::GtfsData& data = reader->getData().get();
  const auto transferFrom = data.getTransfers("1100079");
  ASSERT_EQ(transferFrom[0]->toStopId, "8014441");
}

TEST_F(GtfsCsvReaderStrategiesTest, testTripReaderTxt) {
  auto strategy = std::vector<std::function<void(schedule::gtfs::GtfsReader&)>>();
  const std::function<void(schedule::gtfs::GtfsReader&)> strategyReader =  readerFactory->getStrategy(GtfsStrategyType::TRIP);
  strategy.push_back(strategyReader);
  reader = std::make_unique<schedule::gtfs::GtfsReader>(std::move(strategy));
  reader->readData();

  const schedule::gtfs::GtfsData& data = reader->getData().get();

  ASSERT_TRUE(data.trips.empty() == false);

  const auto trip = data.getTrip("106.TA.91-10-A-j24-1.7.H");

  ASSERT_EQ(trip->routeId, "91-10-A-j24-1");
  ASSERT_EQ(trip->serviceId, "TA+p60e0");
}

TEST_F(GtfsCsvReaderStrategiesTest, testrouteReaderTxt) {
  auto strategy = std::vector<std::function<void(schedule::gtfs::GtfsReader&)>>();
  const std::function<void(schedule::gtfs::GtfsReader&)> strategyReader =  readerFactory->getStrategy(GtfsStrategyType::ROUTE);;
  strategy.push_back(strategyReader);
  reader = std::make_unique<schedule::gtfs::GtfsReader>(std::move(strategy));
  reader->readData();
  const schedule::gtfs::GtfsData& data = reader->getData().get();
  const auto route = data.getRoute("91-10-E-j24-1");
  ASSERT_TRUE(route->routeId.empty() == false);
  ASSERT_EQ(route->routeId, "91-10-E-j24-1");
  ASSERT_EQ(static_cast<int>(route->routeType), 900);
}

TEST(GTFS, TestStrategyReader) {
  using namespace std::literals::string_literals;
  std::string basePath = TEST_DATA_DIR;

  const auto readerFactory = schedule::gtfs::createGtfsReaderStrategyFactory(schedule::gtfs::ReaderType::TXT, std::move(basePath));

  getLogger(Target::CONSOLE, LoggerName::GTFS)->setLevel(LoggerBridge::ERR);


  // create strategy callable objects
  const auto agencyStrategy = readerFactory->getStrategy(GtfsStrategyType::AGENCY);
  const auto calendarStrategy = readerFactory->getStrategy(GtfsStrategyType::CALENDAR);
  const auto calendarDatesStrategy = readerFactory->getStrategy(GtfsStrategyType::CALENDAR_DATE);
  const auto routesStrategy = readerFactory->getStrategy(GtfsStrategyType::ROUTE);
  const auto stopStrategy = readerFactory->getStrategy(GtfsStrategyType::STOP);
  const auto stopTimeStrategy = readerFactory->getStrategy(GtfsStrategyType::STOP_TIME);
  const auto transferStrategy = readerFactory->getStrategy(GtfsStrategyType::TRANSFER);
  const auto tripStrategy = readerFactory->getStrategy(GtfsStrategyType::TRIP);

  std::vector strategies = {agencyStrategy, calendarStrategy, calendarDatesStrategy, routesStrategy, stopStrategy, stopTimeStrategy, transferStrategy, tripStrategy};

  const std::unique_ptr<schedule::DataReader<schedule::DataContainer<schedule::gtfs::GtfsData>>> reader = std::make_unique<schedule::gtfs::GtfsReader>(std::move(strategies));
  reader->readData();
  const auto data = reader->getData().get();
  //
   EXPECT_FALSE(data.agencies.empty() || data.calendars.empty()
                || data.calendarDates.empty() || data.routes.empty() || data.stops.empty()
                || data.stopTimes.empty() || data.transfers.empty() || data.trips.empty());
}

