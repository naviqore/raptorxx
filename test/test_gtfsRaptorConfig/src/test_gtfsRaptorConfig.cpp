//
// Created by MichaelBrunner on 30/08/2024.
//

#include "DataReader.h"
#include "GtfsData.h"
#include "GtfsReader.h"
#include "GtfsReaderStrategyFactory.h"
#include "GtfsToRaptorConverter.h"
#include "Leg.h"
#include "LocalDateTime.h"

#if LOGGER
#include "LoggerFactory.h"
#endif


#include <DataContainer.h>
#include <gtest/gtest.h>


auto getTestLocalDateTime()
{
  return raptor::utils::LocalDateTime{std::chrono::year{2024},
                                      std::chrono::month{4},
                                      std::chrono::day{26},
                                      std::chrono::hours{8},
                                      std::chrono::minutes{0},
                                      std::chrono::seconds{0}};
}

class GtfsRaptorConfigTest : public ::testing::Test {
protected:
  std::unique_ptr<schedule::DataReader<schedule::DataContainer<schedule::gtfs::GtfsData>>> reader;
  std::unique_ptr<schedule::gtfs::IGtfsReaderStrategyFactory> readerFactory;
  std::string basePath = TEST_DATA_DIR;
  schedule::gtfs::GtfsData data;
  static constexpr int defaultSameStopTransferTime{120};

  void SetUp() override
  {
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

TEST_F(GtfsRaptorConfigTest, shouldConvertGtfsScheduleToRaptor)
{
  const auto dateTime = getTestLocalDateTime();

  auto timetableManager = std::make_unique<converter::TimetableManager>(std::move(data), dateTime);
  auto mapper = converter::GtfsToRaptorConverter(defaultSameStopTransferTime, std::move(timetableManager));
  const auto raptor = mapper.convert();

  ASSERT_TRUE(raptor != nullptr);
}

TEST_F(GtfsRaptorConfigTest, routeFromVonwilToStephanshorn)
{
  // Arrange
  const auto dateTime = getTestLocalDateTime();

  auto timetableManager = std::make_unique<converter::TimetableManager>(std::move(data), dateTime);
  auto mapper = converter::GtfsToRaptorConverter(defaultSameStopTransferTime, std::move(timetableManager));
  const auto raptor = mapper.convert();

  const auto queryConfig = raptor::config::QueryConfig();
  const auto raptorRouter = raptor::RaptorRouter(std::move(*raptor));
  // Act
  const auto connections = raptorRouter.routeEarliestArrival(
    {{"8589640", static_cast<raptor::types::raptorInt>(dateTime.secondsOfDay())}},
    {{"8589631", 0}},
    queryConfig);

  const auto& firstConnection = connections.at(0);

  const auto& leg1 = firstConnection->getLegs().at(0);
  // Assert
  ASSERT_EQ(leg1->getFromStopId(), "8589640");
  ASSERT_EQ(leg1->getToStopId(), "8574471");

  const auto& leg2 = firstConnection->getLegs().at(1);
  ASSERT_EQ(leg2->getFromStopId(), "8574471");
  ASSERT_EQ(leg2->getToStopId(), "8589631");
}

// "8588889","Abtwil SG, Dorf"
// "8588889","St. Gallen, Kantonsspital"
TEST_F(GtfsRaptorConfigTest, routeFromAbtwilDorfToWestcenter)
{
  // Arrange
  const auto dateTime = getTestLocalDateTime();

  auto timetableManager = std::make_unique<converter::TimetableManager>(std::move(data), dateTime);
  auto mapper = converter::GtfsToRaptorConverter(defaultSameStopTransferTime, std::move(timetableManager));
  const auto raptor = mapper.convert();
  const auto raptorRouter = raptor::RaptorRouter(std::move(*raptor));
  // Act
  const auto startTime = std::chrono::high_resolution_clock::now();
  const auto connections = raptorRouter.routeEarliestArrival(
    {{"8588889", static_cast<raptor::types::raptorInt>(dateTime.secondsOfDay())}},
    {{"8589644", 0}},
    {});

  const auto endTime = std::chrono::high_resolution_clock::now();
  const std::chrono::duration<double, std::milli> fp_ms = endTime - startTime;
  const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();

  std::cout << "Time spent for routing fpms: " << fp_ms << " milliseconds" << '\n';
  std::cout << "Time spent for routing: " << duration << " milliseconds" << '\n';

  const auto& firstConnection = connections.at(0);

  const auto& leg1 = firstConnection->getLegs().at(0);
  ASSERT_EQ(leg1->getFromStopId(), "8588889");
  ASSERT_EQ(leg1->getToStopId(), "8587343");

  const auto& leg2 = firstConnection->getLegs().at(1);
  ASSERT_EQ(leg2->getFromStopId(), "8587343");
  ASSERT_EQ(leg2->getToStopId(), "8589644");

  const auto& secondConnection = connections.at(1);

  const auto& leg3 = secondConnection->getLegs().at(0);
  ASSERT_EQ(leg3->getFromStopId(), "8588889");
  ASSERT_EQ(leg3->getToStopId(), "8589896:0:B");

  const auto& leg4 = secondConnection->getLegs().at(1);
  ASSERT_EQ(leg4->getFromStopId(), "8589896:0:B");
  ASSERT_EQ(leg4->getToStopId(), "8506300:0:2");

  const auto& leg5 = secondConnection->getLegs().at(2);
  ASSERT_EQ(leg5->getFromStopId(), "8506300:0:2");
  ASSERT_EQ(leg5->getToStopId(), "8506301:0:1");

  const auto& leg6 = secondConnection->getLegs().at(3);
  ASSERT_EQ(leg6->getFromStopId(), "8506301:0:1");
  ASSERT_EQ(leg6->getToStopId(), "8587343");

  const auto& leg7 = secondConnection->getLegs().at(4);
  ASSERT_EQ(leg7->getFromStopId(), "8587343");
  ASSERT_EQ(leg7->getToStopId(), "8589644");
}

TEST_F(GtfsRaptorConfigTest, routeFromSpeicherArToHaggen)
{
  // Arrange
  const auto dateTime = getTestLocalDateTime();

  auto timetableManager = std::make_unique<converter::TimetableManager>(std::move(data), dateTime);
  auto mapper = converter::GtfsToRaptorConverter(defaultSameStopTransferTime, std::move(timetableManager));
  const auto raptor = mapper.convert();

  const auto raptorRouter = raptor::RaptorRouter(std::move(*raptor));
  // Act
  const auto connections = raptorRouter.routeEarliestArrival(
    {{"8506366", static_cast<raptor::types::raptorInt>(dateTime.secondsOfDay())}},
    {{"8589582", 0}},
    {});

  const auto& firstConnection = connections.at(0);

  const auto& leg1 = firstConnection->getLegs().at(0);
  ASSERT_EQ(leg1->getFromStopId(), "8506366");
  ASSERT_EQ(leg1->getToStopId(), "8574075");

  const auto& leg2 = firstConnection->getLegs().at(1);
  ASSERT_EQ(leg2->getFromStopId(), "8574075");
  ASSERT_EQ(leg2->getToStopId(), "8506654");

  const auto& leg3 = firstConnection->getLegs().at(2);
  ASSERT_EQ(leg3->getFromStopId(), "8506654");
  ASSERT_EQ(leg3->getToStopId(), "8580689:0:B");

  const auto& leg4 = firstConnection->getLegs().at(3);
  ASSERT_EQ(leg4->getFromStopId(), "8580689:0:B");
  ASSERT_EQ(leg4->getToStopId(), "8589582");

  const auto& secondConnection = connections.at(1);

  const auto& leg5 = secondConnection->getLegs().at(0);
  ASSERT_EQ(leg5->getFromStopId(), "8506366");
  ASSERT_EQ(leg5->getToStopId(), "8574075");

  const auto& leg6 = secondConnection->getLegs().at(1);
  ASSERT_EQ(leg6->getFromStopId(), "8574075");
  ASSERT_EQ(leg6->getToStopId(), "8574066");

  const auto& leg7 = secondConnection->getLegs().at(2);
  ASSERT_EQ(leg7->getFromStopId(), "8574066");
  ASSERT_EQ(leg7->getToStopId(), "8506375:0:3");

  const auto& leg8 = secondConnection->getLegs().at(3);
  ASSERT_EQ(leg8->getFromStopId(), "8506375:0:3");
  ASSERT_EQ(leg8->getToStopId(), "8506302:0:12");

  const auto& leg9 = secondConnection->getLegs().at(4);
  ASSERT_EQ(leg9->getFromStopId(), "8506302:0:12");
  ASSERT_EQ(leg9->getToStopId(), "8506302:0:6");

  const auto& leg10 = secondConnection->getLegs().at(5);
  ASSERT_EQ(leg10->getFromStopId(), "8506302:0:6");
  ASSERT_EQ(leg10->getToStopId(), "8506392:0:3");

  const auto& leg11 = secondConnection->getLegs().at(6);
  ASSERT_EQ(leg11->getFromStopId(), "8506392:0:3");
  ASSERT_EQ(leg11->getToStopId(), "8589582");
}

// "8579880","Heiligkreuz (Mels), Untergasse"
// "8587965","Erlenbach ZH, Bahnhof"
TEST_F(GtfsRaptorConfigTest, routeFromHeiligkreuzToErlenbach)
{
  // Arrange
  const auto dateTime = getTestLocalDateTime();

  auto timetableManager = std::make_unique<converter::TimetableManager>(std::move(data), dateTime);
  auto mapper = converter::GtfsToRaptorConverter(defaultSameStopTransferTime, std::move(timetableManager));
  const auto raptor = mapper.convert();

  const auto queryConfig = raptor::config::QueryConfig();
  const auto raptorRouter = raptor::RaptorRouter(std::move(*raptor));
  // Act

  const auto connections = raptorRouter.routeEarliestArrival(
    {{"8579880", static_cast<raptor::types::raptorInt>(dateTime.secondsOfDay())}},
    {{"8587965", static_cast<raptor::types::raptorInt>(0)}},
    queryConfig);

  const auto& firstConnection = connections.at(0);

  const auto& leg1 = firstConnection->getLegs().at(0);
  ASSERT_EQ(leg1->getFromStopId(), "8579880");
  ASSERT_EQ(leg1->getToStopId(), "8574614:0:D");

  const auto& leg2 = firstConnection->getLegs().at(1);
  ASSERT_EQ(leg2->getFromStopId(), "8574614:0:D");
  ASSERT_EQ(leg2->getToStopId(), "8509411:0:2");

  const auto& leg3 = firstConnection->getLegs().at(2);
  ASSERT_EQ(leg3->getFromStopId(), "8509411:0:2");
  ASSERT_EQ(leg3->getToStopId(), "8503000:0:5");

  const auto& leg4 = firstConnection->getLegs().at(3);
  ASSERT_EQ(leg4->getFromStopId(), "8503000:0:5");
  ASSERT_EQ(leg4->getToStopId(), "8503000:0:43/44");

  const auto& leg5 = firstConnection->getLegs().at(4);
  ASSERT_EQ(leg5->getFromStopId(), "8503000:0:43/44");
  ASSERT_EQ(leg5->getToStopId(), "8503102:0:3");

  const auto& leg6 = firstConnection->getLegs().at(5);
  ASSERT_EQ(leg6->getFromStopId(), "8503102:0:3");
  ASSERT_EQ(leg6->getToStopId(), "8587965");

  const auto& secondConnection = connections.at(1);

  const auto& leg7 = secondConnection->getLegs().at(0);
  ASSERT_EQ(leg7->getFromStopId(), "8579880");
  ASSERT_EQ(leg7->getToStopId(), "8509650");

  const auto& leg8 = secondConnection->getLegs().at(1);
  ASSERT_EQ(leg8->getFromStopId(), "8509650");
  ASSERT_EQ(leg8->getToStopId(), "8578567");

  const auto& leg9 = secondConnection->getLegs().at(2);
  ASSERT_EQ(leg9->getFromStopId(), "8578567");
  ASSERT_EQ(leg9->getToStopId(), "8509414:0:4");

  const auto& leg10 = secondConnection->getLegs().at(3);
  ASSERT_EQ(leg10->getFromStopId(), "8509414:0:4");
  ASSERT_EQ(leg10->getToStopId(), "8503202:0:5");

  const auto& leg11 = secondConnection->getLegs().at(4);
  ASSERT_EQ(leg11->getFromStopId(), "8503202:0:5");
  ASSERT_EQ(leg11->getToStopId(), "8503674");

  const auto& leg12 = secondConnection->getLegs().at(5);
  ASSERT_EQ(leg12->getFromStopId(), "8503674");
  ASSERT_EQ(leg12->getToStopId(), "8503659");

  const auto& leg13 = secondConnection->getLegs().at(6);
  ASSERT_EQ(leg13->getFromStopId(), "8503659");
  ASSERT_EQ(leg13->getToStopId(), "8587965");
}

// "8579885","Mels, Bahnhof"
TEST_F(GtfsRaptorConfigTest, routeStGallenVonwilToMels)
{
#if LOGGER
  getConsoleLogger(LoggerName::RAPTOR)->setLevel(LoggerBridge::OFF);
#endif

  // Arrange
  const auto dateTime = getTestLocalDateTime();

  auto timetableManager = std::make_unique<converter::TimetableManager>(std::move(data), dateTime);
  auto mapper = converter::GtfsToRaptorConverter(defaultSameStopTransferTime, std::move(timetableManager));
  const auto raptor = mapper.convert();
  const auto raptorRouter = raptor::RaptorRouter(std::move(*raptor));

  // Act
  const auto startTime = std::chrono::high_resolution_clock::now();
  const auto connections = raptorRouter.routeEarliestArrival(
    {{"8589640", static_cast<raptor::types::raptorInt>(dateTime.secondsOfDay())}},
    {{"8579885", 0}},
    {});

  const auto endTime = std::chrono::high_resolution_clock::now();
  const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();

  std::cout << "Time spent for routing: " << duration << " milliseconds" << '\n';
  std::cout << "Connections: " << connections.size() << '\n';

  // Assert
  const auto& firstConnection = connections.front();

  const auto& leg1 = firstConnection->getLegs().at(0);
  ASSERT_EQ(leg1->getFromStopId(), "8589640");
  ASSERT_EQ(leg1->getToStopId(), "8574095:0:E");

  const auto& leg2 = firstConnection->getLegs().at(1);
  ASSERT_EQ(leg2->getFromStopId(), "8574095:0:E");
  ASSERT_EQ(leg2->getToStopId(), "8506302:0:5");

  const auto& leg3 = firstConnection->getLegs().at(2);
  ASSERT_EQ(leg3->getFromStopId(), "8506302:0:5");
  ASSERT_EQ(leg3->getToStopId(), "8509411:0:4");

  const auto& leg4 = firstConnection->getLegs().at(3);
  ASSERT_EQ(leg4->getFromStopId(), "8509411:0:4");
  ASSERT_EQ(leg4->getToStopId(), "8574614:0:G");

  const auto& leg5 = firstConnection->getLegs().at(4);
  ASSERT_EQ(leg5->getFromStopId(), "8574614:0:G");
  ASSERT_EQ(leg5->getToStopId(), "8579885");
}

int main(int argc, char** argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}