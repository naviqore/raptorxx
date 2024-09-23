//
// Created by MichaelBrunner on 01/09/2024.
//


#include "DataReader.h"
#include "GtfsData.h"
#include "GtfsReader.h"
#include "GtfsReaderStrategyFactory.h"
#include "GtfsToRaptorConverter.h"
#include "LocalDateTime.h"

#include <DataContainer.h>
#include <gtest/gtest.h>

#ifdef LOGGER
#pragma message("LOGGER is defined")
#include "LoggerFactory.h"
#else
#pragma message("LOGGER is not defined")
#endif


class GtfsRoutePartitionerTest : public ::testing::Test {
protected:
  std::unique_ptr<schedule::DataReader<schedule::DataContainer<schedule::gtfs::GtfsData>>> reader;
  std::unique_ptr<schedule::gtfs::IGtfsReaderStrategyFactory> readerFactory;
  std::string basePath = TEST_DATA_DIR;
  schedule::gtfs::GtfsData data;

  void SetUp() override
  {

#ifdef LOGGER
    getConsoleLogger(LoggerName::RAPTOR)->info("Setup Raptor");
#endif

    readerFactory = schedule::gtfs::createGtfsReaderStrategyFactory(schedule::gtfs::ReaderType::CSV_PARALLEL, std::move(basePath));

    const auto calendarStrategy = readerFactory->getStrategy(GtfsStrategyType::CALENDAR);
    const auto calendarDatesStrategy = readerFactory->getStrategy(GtfsStrategyType::CALENDAR_DATE);
    const auto routesStrategy = readerFactory->getStrategy(GtfsStrategyType::ROUTE);
    const auto stopStrategy = readerFactory->getStrategy(GtfsStrategyType::STOP);
    const auto stopTimeStrategy = readerFactory->getStrategy(GtfsStrategyType::STOP_TIME);
    const auto transferStrategy = readerFactory->getStrategy(GtfsStrategyType::TRANSFER);
    const auto tripStrategy = readerFactory->getStrategy(GtfsStrategyType::TRIP);

    std::vector strategies = {calendarStrategy,
                              calendarDatesStrategy,
                              routesStrategy,
                              stopStrategy,
                              stopTimeStrategy,
                              transferStrategy,
                              tripStrategy};
    reader = std::make_unique<schedule::gtfs::GtfsReader>(std::move(strategies));
    reader->readData();
    this->data = reader->getData().get();
  }

  void TearDown() override
  {
    reader.reset(nullptr);
  }
};

TEST_F(GtfsRoutePartitionerTest, shouldConvertGtfsScheduleToRaptor)
{
  // Arrange
  const auto dateTime = raptor::utils::LocalDateTime{std::chrono::year{2024},
                                                     std::chrono::month{4},
                                                     std::chrono::day{26},
                                                     std::chrono::hours{8},
                                                     std::chrono::minutes{0},
                                                     std::chrono::seconds{0}};

  auto timetableManager = std::make_unique<converter::TimetableManager>(std::move(data), dateTime);
  auto mapper = converter::GtfsToRaptorConverter(0, std::move(timetableManager));
  std::ignore = mapper.convert();
  const auto& routePartitioner = mapper.getRoutePartitioner();
  // Act
  const auto& subRoute1 = routePartitioner.getSubRoute("112.TA.96-167-1-j24-1.22.R");

  // Assert
  ASSERT_TRUE(false == subRoute1.getTrips().empty());
  ASSERT_STREQ(subRoute1.getRouteId().c_str(), "96-167-1-j24-1");
  ASSERT_STREQ(subRoute1.getStopsSequence().front()->stopId.c_str(), "8503582");
  ASSERT_STREQ(subRoute1.getStopsSequence().back()->stopId.c_str(), "8572550:0:A");
  ASSERT_TRUE(subRoute1.getTrips().size() == 9);

  // Act
  const auto& subRoute2 = routePartitioner.getSubRoute("170.TA.92-13-H-j24-1.1.H");

  // Assert
  ASSERT_STREQ(subRoute2.getRouteId().c_str(), "92-13-H-j24-1");
  ASSERT_STREQ(subRoute2.getStopsSequence().front()->stopId.c_str(), "8579253");
  ASSERT_STREQ(subRoute2.getStopsSequence().back()->stopId.c_str(), "8592129");
  ASSERT_TRUE(subRoute2.getTrips().size() == 22);
}
