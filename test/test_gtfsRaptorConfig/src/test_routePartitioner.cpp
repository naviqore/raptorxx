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

    std::vector strategies = {calendarStrategy, calendarDatesStrategy, routesStrategy, stopStrategy, stopTimeStrategy, transferStrategy, tripStrategy};
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
  const auto START_OF_DAY{raptor::utils::LocalDateTime{2024y, std::chrono::January, 11d, 0h, 0min, 0s}};
  const auto timeTableManager = converter::TimetableManager(std::move(data), START_OF_DAY);
  const auto& routePartitioner = timeTableManager.getRoutePartitioner();

  const auto& subRoute = routePartitioner.getSubRoute("229.TA.92-2-B-j24-1.22.H");

  ASSERT_TRUE(false == subRoute.getTrips().empty());
  ASSERT_STREQ(subRoute.getRouteId().c_str(), "92-2-B-j24-1");
  ASSERT_STREQ(subRoute.getStopsSequence().front()->stopId.c_str(), "8580689:0:A"); // St. Gallen, Neudorf/R'str.
  ASSERT_STREQ(subRoute.getStopsSequence().back()->stopId.c_str(), "8589644");      // St. Gallen, Westcenter
  ASSERT_TRUE(subRoute.getTrips().size() == 279);
}
