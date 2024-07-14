//
// Created by MichaelBrunner on 27/06/2024.
//


#include "DataReader.h"
#include "IRaptorAlgorithmFactory.h"
#include "LoggerFactory.h"
#include "Raptor.h"
#include "RaptorAlgorithmFactory.h"
#include "raptorTypes.h"
#include "gtfs/GtfsReaderStrategyFactory.h"
#include "gtfs/strategies/GtfsAgencyReader.h"
#include "gtfs/strategies/GtfsCalendarReader.h"
#include "gtfs/strategies/GtfsRouteReader.h"
#include "gtfs/strategies/GtfsStopReader.h"
#include "gtfs/strategies/GtfsStopTimeReader.h"
#include "gtfs/strategies/GtfsTransferReader.h"
#include "gtfs/strategies/GtfsTripReader.h"

#include <gtfs/GtfsReader.h>

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

  std::unique_ptr<schedule::gtfs::GtfsReaderStrategyFactory> readerFactory;

  std::map<schedule::gtfs::utils::GTFS_FILE_TYPE, std::string> lFileNameMap;

  void SetUp() override {
    basePath.back() == '/' ? basePath : basePath += '/';
    readerFactory = std::make_unique<schedule::gtfs::GtfsReaderStrategyFactory>(std::move(basePath));
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
  const auto routesStrategy = readerFactory->getStrategy(schedule::gtfs::GtfsReaderStrategyFactory::Type::ROUTE);
  const auto stopStrategy = readerFactory->getStrategy(schedule::gtfs::GtfsReaderStrategyFactory::Type::STOP);
  const auto stopTimeStrategy = readerFactory->getStrategy(schedule::gtfs::GtfsReaderStrategyFactory::Type::STOP_TIME);
  const auto transferStrategy = readerFactory->getStrategy(schedule::gtfs::GtfsReaderStrategyFactory::Type::TRANSFER);
  const auto tripStrategy = readerFactory->getStrategy(schedule::gtfs::GtfsReaderStrategyFactory::Type::TRIP);

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
  const auto calendarStrategy = readerFactory->getStrategy(schedule::gtfs::GtfsReaderStrategyFactory::Type::CALENDAR);
  const auto calendarDatesStrategy = readerFactory->getStrategy(schedule::gtfs::GtfsReaderStrategyFactory::Type::CALENDAR_DATE);
  const auto routesStrategy = readerFactory->getStrategy(schedule::gtfs::GtfsReaderStrategyFactory::Type::ROUTE);
  const auto stopStrategy = readerFactory->getStrategy(schedule::gtfs::GtfsReaderStrategyFactory::Type::STOP);
  const auto stopTimeStrategy = readerFactory->getStrategy(schedule::gtfs::GtfsReaderStrategyFactory::Type::STOP_TIME);
  const auto transferStrategy = readerFactory->getStrategy(schedule::gtfs::GtfsReaderStrategyFactory::Type::TRANSFER);
  const auto tripStrategy = readerFactory->getStrategy(schedule::gtfs::GtfsReaderStrategyFactory::Type::TRIP);

  std::vector strategies = {/*agencyStrategy,*/ calendarStrategy, calendarDatesStrategy, routesStrategy, stopStrategy, stopTimeStrategy, transferStrategy, tripStrategy}; //

  const std::unique_ptr<schedule::DataReader<schedule::DataContainer<schedule::gtfs::GtfsData>>> reader = std::make_unique<schedule::gtfs::GtfsReader>(std::move(strategies));
  reader->readData();

  auto relationManager = schedule::gtfs::RelationManager(std::move(reader->getData().get()));

  //// "BEATTY_AIRPORT_STATION","Nye County Airport Station (Demo)","36.868446","-116.784582","","","1235","1",""
  auto stopIdSources = relationManager.getStopIdsFromStopName("St. Gallen, Vonwil"); // Zürich, Bahnhof Stadelhofen
  ASSERT_TRUE(!stopIdSources.empty());

  auto stopIdTargets = relationManager.getStopIdsFromStopName("St. Gallen,Klinik Stephanshorn"); // Zürich, Schiffbau
  ASSERT_TRUE(!stopIdTargets.empty());
  std::unordered_map<std::string, std::vector<std::string>> routeStops;

  // Log all stops for each trip that stops at the source stop
  // getLogger(Target::CONSOLE, LoggerName::RAPTOR)->info("Log all stops for each trip that stops at the source stop");
  // for (const auto& stopId : stopIdSources)
  // {
  //   for (const auto& stopTimes = relationManager.getData().stopTimes.at(stopId);
  //        const auto& stopTime : stopTimes)
  //   {
  //     for (const auto& trips = relationManager.getData().trips.at(stopTime.tripId);
  //          const auto& trip : trips)
  //     {
  //       getLogger(Target::CONSOLE, LoggerName::RAPTOR)->info(" ");
  //       getLogger(Target::CONSOLE, LoggerName::RAPTOR)->info(std::format("Trip: {}", trip.tripId));
  //
  //       std::ranges::for_each(trip.stopTimes, [&](const auto& stopTimeItem) {
  //         getLogger(Target::CONSOLE, LoggerName::RAPTOR)->info(std::format("Stop: {} {}", stopTimeItem.stopId, relationManager.getStopNameFromStopId(stopTimeItem.stopId)));
  //       });
  //     }
  //   }
  // }

  getLogger(Target::CONSOLE, LoggerName::RAPTOR)->info(" ------------------------- ");
  getLogger(Target::CONSOLE, LoggerName::RAPTOR)->info(" ");

  const std::unique_ptr<raptor::strategy::factory::IRaptorAlgorithmFactory> factory = std::make_unique<raptor::strategy::factory::RaptorAlgorithmFactory>();
  ASSERT_TRUE(factory);

  auto strategy = factory->create(raptor::strategy::factory::IRaptorAlgorithmFactory::RAPTOR, std::move(relationManager));
  std::unique_ptr<raptor::IRaptor> raptor = std::make_unique<raptor::Raptor>(std::move(strategy));
  ASSERT_TRUE(raptor);

  const auto connectionRequest = raptor::utils::ConnectionRequest{std::move(stopIdSources), std::move(stopIdTargets), 52'920}; // 14:42:00

  auto connections = raptor->getConnections(connectionRequest);
}
