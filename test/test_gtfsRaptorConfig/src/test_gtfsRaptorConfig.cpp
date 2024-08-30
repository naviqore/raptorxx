//
// Created by MichaelBrunner on 30/08/2024.
//

#include "DataReader.h"
#include "GtfsData.h"
#include "GtfsReader.h"
#include "GtfsReaderStrategyFactory.h"
#include "GtfsToRaptorConverter.h"
#include "utils/DataTypeConverter.h"
#include <DataContainer.h>


#include <gtest/gtest.h>

class GtfsRaptorConfigTest : public ::testing::Test {
protected:
  std::unique_ptr<schedule::DataReader<schedule::DataContainer<schedule::gtfs::GtfsData>>> reader;
  std::unique_ptr<schedule::gtfs::IGtfsReaderStrategyFactory> readerFactory;
  std::string basePath = TEST_DATA_DIR;
  schedule::gtfs::GtfsData data;

  void SetUp() override
  {
    readerFactory = schedule::gtfs::createGtfsReaderStrategyFactory(schedule::gtfs::ReaderType::CSV_PARALLEL, std::move(basePath));

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
    this->data = reader->getData().get();
  }

  void TearDown() override
  {
    reader.reset(nullptr);
  }
};

TEST_F(GtfsRaptorConfigTest, shouldConvertGtfsScheduleToRaptor)
{
  auto mapper = converter::GtfsToRaptorConverter(std::move(data), 0);
  const auto raptor = mapper.convert();
  ASSERT_TRUE(raptor != nullptr);
}