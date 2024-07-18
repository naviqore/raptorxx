//
// Created by MichaelBrunner on 27/06/2024.
//


#include "DataReader.h"
#include "GtfsReaderStrategyFactory.h"
#include "IRaptorAlgorithmFactory.h"
#include "LoggerFactory.h"
#include "Raptor.h"
#include "RaptorAlgorithmFactory.h"
#include "raptorTypes.h"
#include "gtfs/GtfsTxtReaderStrategyFactory.h"

#include <GtfsReader.h>

#include <gtest/gtest.h>
#include <gmock/gmock.h>


////////// TEST ROUTES //////////

////             O (stopA)
////             |
////             O (stopB)
////             |\
////             | \
////             O  O (stopC)
////             |  |
////             |  O (stopD)
////             |  | \
////             |  |  \
////             |  O   O (stopE)
////             |      |
////             |      O (stopG)
////             |     / \
////             |    /   \
////             O   O     O (stopF)
////              \ /       \
////               O         \
////                         O (stopH)
////               |
////               O (stopI)
////               |
////               O (stopJ)
////               |
////               O (stopK)
////               |
////               O (stopL)
///


///         O (Furnace Creek Resort)
///          |
///          O (Nye County Airport)
///          |
///          O (Bullfrog) ------------ O (Stagecoach Hotel & Casino)
///                  \                           /
///                   \                         /
///                    \                       /
///                     \                     /
///                      O (North Ave / D Ave N)
///                       |
///                       O (North Ave / N A Ave)
///                       |
///                       O (Doing Ave / D Ave N)
///                       |
///                       O (E Main St / S Irving St)
///                       |
///                       O (Amargosa Valley)
///



class RaptorTest : public testing::Test
{
protected:
  std::unique_ptr<schedule::DataReader<schedule::DataContainer<schedule::gtfs::GtfsData>>> reader;
  // raptor::utils::ConnectionRequest request;

  std::string basePath = TEST_DATA_DIRRECTORY;

  std::unique_ptr<schedule::gtfs::IGtfsReaderStrategyFactory> readerFactory;

  std::map<schedule::gtfs::utils::GTFS_FILE_TYPE, std::string> lFileNameMap;

  void SetUp() override {
    basePath.back() == '/' ? basePath : basePath += '/';
    readerFactory = schedule::gtfs::createGtfsReaderStrategyFactory(schedule::gtfs::ReaderType::CSV_PARALLEL, std::move(basePath));
  }

  void TearDown() override {
    std::cout << "GtfsReaderStrategiesTest::TearDown()" << '\n';
    reader.reset(nullptr);
  }
};


TEST_F(RaptorTest, InitialTest) {


  // const auto agencyStrategy = readerFactory->getStrategy(schedule::gtfs::GtfsReaderStrategyFactory::Type::AGENCY);
  // const auto calendarStrategy = readerFactory->getStrategy(schedule::gtfs::GtfsReaderStrategyFactory::Type::CALENDAR);
  // const auto calendarDatesStrategy = readerFactory->getStrategy(schedule::gtfs::GtfsReaderStrategyFactory::Type::CALENDAR_DATE);
  const auto routesStrategy = readerFactory->getStrategy(GtfsStrategyType::ROUTE);
  const auto stopStrategy = readerFactory->getStrategy(GtfsStrategyType::STOP);
  const auto stopTimeStrategy = readerFactory->getStrategy(GtfsStrategyType::STOP_TIME);
  const auto transferStrategy = readerFactory->getStrategy(GtfsStrategyType::TRANSFER);
  const auto tripStrategy = readerFactory->getStrategy(GtfsStrategyType::TRIP);

  std::vector strategies = {/*agencyStrategy, calendarStrategy, calendarDatesStrategy,*/ routesStrategy, stopStrategy, stopTimeStrategy, transferStrategy, tripStrategy}; //

  const std::unique_ptr<schedule::DataReader<schedule::DataContainer<schedule::gtfs::GtfsData>>> reader = std::make_unique<schedule::gtfs::GtfsReader>(std::move(strategies));
  reader->readData();
  auto relationManager = schedule::gtfs::RelationManager(std::move(reader->getData().get()));

  // auto stopIdSource = relationManager.getStopIdFromStopName("Zürich, Bahnhof Affoltern");
  // ASSERT_STREQ(stopIdSource.c_str(), "ch:1:sloid:91054::22");

  // auto stopIdSource = relationManager.getStopIdFromStopName("Meilen, Bahnhof"); // "Erlenbach ZH, Trottgatter"
  // auto stopIdSources = relationManager.getStopIdsFromStopName("Küsnacht ZH, Bahnhof");
  //
  // auto stopIdTarget = relationManager.getStopIdFromStopName("Kempraten, Bahnhof"); // "Erlenbach ZH, Bahnhof "Erlenbach ZH, Erlenhöhe""
  // // ASSERT_STREQ(stopIdTarget.c_str(), "ch:1:sloid:87965::50");
  // auto stopIdTargets = relationManager.getStopIdsFromStopName("Erlenbach ZH, Bahnhof"); // "Erlenbach ZH, Erlenhöhe""Meilen, Bahnhof"Zürich, Seebach

  auto stopIdSources = relationManager.getStopIdsFromStopName("Zürich, Holzerhurd");

  auto stopIdTargets = relationManager.getStopIdsFromStopName("Zürich, Bahnhof Stadelhofen");

  // stopIdTarget = relationManager.getStopIdFromStopName("Zürich, Seebach");
  // ASSERT_TRUE(stopIdTarget.c_str() == "ch:1:sloid:91354::1" || "ch:1:sloid:91354::20" || "ch:1:sloid:91354::21" || "ch:1:sloid:91354::22" || "ch:1:sloid:91354::50");


  const std::unique_ptr<raptor::strategy::factory::IRaptorAlgorithmFactory> factory = std::make_unique<raptor::strategy::factory::RaptorAlgorithmFactory>();
  ASSERT_TRUE(factory);

  auto strategy = factory->create(raptor::strategy::factory::IRaptorAlgorithmFactory::RAPTOR, std::move(relationManager));
  std::unique_ptr<raptor::IRaptor> raptor = std::make_unique<raptor::Raptor>(std::move(strategy));
  ASSERT_TRUE(raptor);

  const auto connectionRequest = raptor::utils::ConnectionRequest{std::move(stopIdSources), std::move(stopIdTargets), 35999}; // 09:59:59

  auto connections = raptor->getConnections(connectionRequest);
}

TEST_F(RaptorTest, RaptorAlgorithm) {

  // const auto agencyStrategy = readerFactory->getStrategy(schedule::gtfs::GtfsReaderStrategyFactory::Type::AGENCY);
  const auto calendarStrategy = readerFactory->getStrategy(GtfsStrategyType::CALENDAR);
  const auto calendarDatesStrategy = readerFactory->getStrategy(GtfsStrategyType::CALENDAR_DATE);
  const auto routesStrategy = readerFactory->getStrategy(GtfsStrategyType::ROUTE);
  const auto stopStrategy = readerFactory->getStrategy(GtfsStrategyType::STOP);
  const auto stopTimeStrategy = readerFactory->getStrategy(GtfsStrategyType::STOP_TIME);
  const auto transferStrategy = readerFactory->getStrategy(GtfsStrategyType::TRANSFER);
  const auto tripStrategy = readerFactory->getStrategy(GtfsStrategyType::TRIP);

  std::vector strategies = {/*agencyStrategy,*/
                            calendarStrategy,
                            calendarDatesStrategy,
                            routesStrategy,
                            stopStrategy,
                            stopTimeStrategy,
                            transferStrategy,
                            tripStrategy}; //

  const auto reader = std::make_unique<schedule::gtfs::GtfsReader>(std::move(strategies));
  reader->readData();

  auto relationManager = schedule::gtfs::RelationManager(std::move(reader->getData().get()));

  auto stopIdSources = relationManager.getStopIdsFromStopName("St. Gallen, Vonwil");
  ASSERT_TRUE(!stopIdSources.empty());

  auto stopIdTargets = relationManager.getStopIdsFromStopName("St. Gallen,Klinik Stephanshorn");
  ASSERT_TRUE(!stopIdTargets.empty());
  std::unordered_map<std::string, std::vector<std::string>> routeStops;

  getLogger(Target::CONSOLE, LoggerName::RAPTOR)->info(" ------------------------- ");
  getLogger(Target::CONSOLE, LoggerName::RAPTOR)->info(" ");

  const auto factory = std::make_unique<raptor::strategy::factory::RaptorAlgorithmFactory>();
  ASSERT_TRUE(factory);

  auto strategy = factory->create(raptor::strategy::factory::IRaptorAlgorithmFactory::RAPTOR, std::move(relationManager));
  std::unique_ptr<raptor::IRaptor> raptor = std::make_unique<raptor::Raptor>(std::move(strategy));
  ASSERT_TRUE(raptor);

  const auto connectionRequest = raptor::utils::ConnectionRequest{std::move(stopIdSources), std::move(stopIdTargets), 52'920}; // 14:42:00

  auto connections = raptor->getConnections(connectionRequest);
}
