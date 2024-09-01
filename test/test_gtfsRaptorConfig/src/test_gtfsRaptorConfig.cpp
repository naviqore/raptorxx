//
// Created by MichaelBrunner on 30/08/2024.
//

#include "DataReader.h"
#include "GtfsData.h"
#include "GtfsReader.h"
#include "GtfsReaderStrategyFactory.h"
#include "GtfsToRaptorConverter.h"
#include "LocalDateTime.h"

#include <DataContainer.h>
#include <gtest/gtest.h>

class GtfsRaptorConfigTest : public ::testing::Test {
protected:
  std::unique_ptr<schedule::DataReader<schedule::DataContainer<schedule::gtfs::GtfsData>>> reader;
  std::unique_ptr<schedule::gtfs::IGtfsReaderStrategyFactory> readerFactory;
  std::string basePath = TEST_DATA_DIR;
  schedule::gtfs::GtfsData data;

  raptor::utils::LocalDateTime START_OF_DAY{raptor::utils::LocalDateTime{2024y, std::chrono::January, 11d, 0h, 0min, 0s}};
  raptor::utils::LocalDateTime EIGHT_AM;
  raptor::utils::LocalDateTime NINE_AM;

  void SetUp() override
  {
    readerFactory = schedule::gtfs::createGtfsReaderStrategyFactory(schedule::gtfs::ReaderType::CSV_PARALLEL, std::move(basePath));

    const auto calendarStrategy = readerFactory->getStrategy(GtfsStrategyType::CALENDAR);
    const auto calendarDatesStrategy = readerFactory->getStrategy(GtfsStrategyType::CALENDAR_DATE);
    const auto routesStrategy = readerFactory->getStrategy(GtfsStrategyType::ROUTE);
    const auto stopStrategy = readerFactory->getStrategy(GtfsStrategyType::STOP);
    const auto stopTimeStrategy = readerFactory->getStrategy(GtfsStrategyType::STOP_TIME);
    // const auto transferStrategy = readerFactory->getStrategy(GtfsStrategyType::TRANSFER);
    const auto tripStrategy = readerFactory->getStrategy(GtfsStrategyType::TRIP);

    std::vector strategies = {calendarStrategy, calendarDatesStrategy, routesStrategy, stopStrategy, stopTimeStrategy /*, transferStrategy*/, tripStrategy};

    reader = std::make_unique<schedule::gtfs::GtfsReader>(std::move(strategies));
    reader->readData();
    auto gtfsData = reader->getData().get();

    auto timetableManager = converter::TimetableManager(std::move(gtfsData));
    this->data = timetableManager.getData();

    EIGHT_AM = START_OF_DAY.addHours(std::chrono::hours(8));
    NINE_AM = START_OF_DAY.addHours(std::chrono::hours(9));
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

TEST_F(GtfsRaptorConfigTest, routeFromVonwilToStephanshorn)
{
  auto mapper = converter::GtfsToRaptorConverter(std::move(data), 0);
  const auto raptor = mapper.convert();

  const auto queryConfig = raptor::config::QueryConfig();
  const auto raptorRouter = raptor::RaptorRouter(std::move(*raptor));
  // Act
  const auto connections = raptorRouter.routeEarliestArrival(
    {{"8589640", static_cast<raptor::types::raptorInt>(EIGHT_AM.secondsOfDay())}},
    {{"8589631", 0}},
    queryConfig);


  ASSERT_TRUE(raptor != nullptr);
}

// "8588889","Abtwil SG, Dorf"
// "8574467","St. Gallen, Kantonsspital"

TEST_F(GtfsRaptorConfigTest, routeFromAbtwilDorfToWestcenter)
{
  auto mapper = converter::GtfsToRaptorConverter(std::move(data), 0);
  const auto raptor = mapper.convert();

  const auto queryConfig = raptor::config::QueryConfig();
  const auto raptorRouter = raptor::RaptorRouter(std::move(*raptor));
  // Act
  const auto connections = raptorRouter.routeEarliestArrival(
    {{"8588889", static_cast<raptor::types::raptorInt>(EIGHT_AM.secondsOfDay())}},
    {{"8589644", 0}},
    queryConfig);


  ASSERT_TRUE(raptor != nullptr);
}