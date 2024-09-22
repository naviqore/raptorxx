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
  const auto dateTime = raptor::utils::LocalDateTime{std::chrono::year{2024}, std::chrono::month{4}, std::chrono::day{26}, std::chrono::hours{8}, std::chrono::minutes{0}, std::chrono::seconds{0}};

  auto timetableManager = std::make_unique<converter::TimetableManager>(std::move(data), dateTime);
  auto mapper = converter::GtfsToRaptorConverter(120, std::move(timetableManager));
  const auto raptor = mapper.convert();

  ASSERT_TRUE(raptor != nullptr);
}

TEST_F(GtfsRaptorConfigTest, routeFromVonwilToStephanshorn)
{
  const auto dateTime = raptor::utils::LocalDateTime{std::chrono::year{2024}, std::chrono::month{4}, std::chrono::day{26}, std::chrono::hours{8}, std::chrono::minutes{0}, std::chrono::seconds{0}};

  auto timetableManager = std::make_unique<converter::TimetableManager>(std::move(data), dateTime);
  auto mapper = converter::GtfsToRaptorConverter(120, std::move(timetableManager));
  const auto raptor = mapper.convert();

  const auto queryConfig = raptor::config::QueryConfig();
  const auto raptorRouter = raptor::RaptorRouter(std::move(*raptor));
  // Act
  const auto connections = raptorRouter.routeEarliestArrival(
    {{"8589640", static_cast<raptor::types::raptorInt>(dateTime.secondsOfDay())}},
    {{"8589631", 0}},
    queryConfig);


  ASSERT_TRUE(raptor != nullptr);
}

// "8588889","Abtwil SG, Dorf"
// "8588889","St. Gallen, Kantonsspital"

TEST_F(GtfsRaptorConfigTest, routeFromAbtwilDorfToWestcenter)
{
  const auto dateTime = raptor::utils::LocalDateTime{std::chrono::year{2024}, std::chrono::month{4}, std::chrono::day{26}, std::chrono::hours{8}, std::chrono::minutes{0}, std::chrono::seconds{0}};

  auto timetableManager = std::make_unique<converter::TimetableManager>(std::move(data), dateTime);
  auto mapper = converter::GtfsToRaptorConverter(120, std::move(timetableManager));
  const auto raptor = mapper.convert();

  const auto queryConfig = raptor::config::QueryConfig();
  const auto raptorRouter = raptor::RaptorRouter(std::move(*raptor));
  // Act
  const auto startTime = std::chrono::high_resolution_clock::now();
  const auto connections = raptorRouter.routeEarliestArrival(
    {{"8588889", static_cast<raptor::types::raptorInt>(dateTime.secondsOfDay())}},
    {{"8589644", 0}},
    queryConfig);

  const auto endTime = std::chrono::high_resolution_clock::now();
  const std::chrono::duration<double, std::milli> fp_ms = endTime - startTime;
  const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();

  std::cout << "Time spent for routing fpms: " << fp_ms << " milliseconds" << '\n';
  std::cout << "Time spent for routing: " << duration << " milliseconds" << '\n';

  ASSERT_TRUE(raptor != nullptr);
}

TEST_F(GtfsRaptorConfigTest, routeFromSpeicherArToHaggen)
{
  const auto dateTime = raptor::utils::LocalDateTime{std::chrono::year{2024}, std::chrono::month{4}, std::chrono::day{26}, std::chrono::hours{8}, std::chrono::minutes{0}, std::chrono::seconds{0}};

  auto timetableManager = std::make_unique<converter::TimetableManager>(std::move(data), dateTime);
  auto mapper = converter::GtfsToRaptorConverter(120, std::move(timetableManager));
  const auto raptor = mapper.convert();

  const auto raptorRouter = raptor::RaptorRouter(std::move(*raptor));
  // Act
  const auto connections = raptorRouter.routeEarliestArrival(
    {{"8506366", static_cast<raptor::types::raptorInt>(dateTime.secondsOfDay())}},
    {{"8589582", 0}},
    {});


  ASSERT_TRUE(raptor != nullptr);
}

// "8579880","Heiligkreuz (Mels), Untergasse","47.0594853159118","9.41034667586818","","Parent8579880"
// "8587965","Erlenbach ZH, Bahnhof","47.305818015385","8.5912448333883","","Parent8587965"
TEST_F(GtfsRaptorConfigTest, routeFromHeiligkreuzToErlenbach)
{
  const auto dateTime = raptor::utils::LocalDateTime{std::chrono::year{2024}, std::chrono::month{4}, std::chrono::day{26}, std::chrono::hours{8}, std::chrono::minutes{0}, std::chrono::seconds{0}};

  auto timetableManager = std::make_unique<converter::TimetableManager>(std::move(data), dateTime);
  auto mapper = converter::GtfsToRaptorConverter(120, std::move(timetableManager));
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
  ASSERT_EQ(leg3->getToStopId(), "8503202:0:5");

  const auto& leg4 = firstConnection->getLegs().at(3);
  ASSERT_EQ(leg4->getFromStopId(), "8503202:0:5");
  ASSERT_EQ(leg4->getToStopId(), "8503674");

  const auto& leg5 = firstConnection->getLegs().at(4);
  ASSERT_EQ(leg5->getFromStopId(), "8503674");
  ASSERT_EQ(leg5->getToStopId(), "8503659");

  const auto& leg6 = firstConnection->getLegs().at(5);
  ASSERT_EQ(leg6->getFromStopId(), "8503659");
  ASSERT_EQ(leg6->getToStopId(), "8587965");


  for (const auto& connection : connections) {

    for (const auto& leg : connection->getLegs()) {
      std::string type = leg->getType().has_value() ? leg->getType().value() == raptor::Leg::Type::WALK_TRANSFER ? "WALK_TRANSFER" : "ROUTE" : "UNKNOWN";
      std::cout << leg->getFromStopId() << " -> " << leg->getToStopId() << type << '\n';
    }
    std::cout << "\n";
  }

  ASSERT_TRUE(raptor != nullptr);
}

// "8579885","Mels, Bahnhof"
TEST_F(GtfsRaptorConfigTest, routeStGallenVonwilToMels)
{
#if LOGGER
  getConsoleLogger(LoggerName::RAPTOR)->setLevel(LoggerBridge::OFF);
#endif

  const auto dateTime = getTestLocalDateTime();

  auto timetableManager = std::make_unique<converter::TimetableManager>(std::move(data), dateTime);
  constexpr int defaultSameStopTransferTime = 120;
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
