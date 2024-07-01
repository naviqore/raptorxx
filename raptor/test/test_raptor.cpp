//
// Created by MichaelBrunner on 27/06/2024.
//


#include "DataReader.h"
#include "IRaptorAlgorithmFactory.h"
#include "Raptor.h"
#include "RaptorAlgorithmFactory.h"
#include "raptorTypes.h"
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



class RaptorTest : public ::testing::Test
{
protected:
  std::unique_ptr<schedule::DataReader<schedule::DataContainer<schedule::gtfs::GtfsData>>> reader;
  // raptor::utils::ConnectionRequest request;

  const std::string basePath = TEST_DATA_DIR_FULL_GTFS;

  std::map<schedule::gtfs::utils::GTFS_FILE_TYPE, std::string> lFileNameMap;

  void SetUp() override {
    std::cout << "GtfsReaderStrategiesTest::SetUp()" << '\n';
    const std::string agencyFile = basePath + "agency.txt";
    const std::string calendarFile = basePath + "calendar.txt";
    const std::string calendarDatesFile = basePath + "calendar_dates.txt";
    const std::string routesFile = basePath + "routes.txt";
    const std::string stopFile = basePath + "stops.txt";
    const std::string stopTimeFile = basePath + "stop_times.txt";
    const std::string TransferFile = basePath + "transfers.txt";
    const std::string TripFile = basePath + "trips.txt";

    lFileNameMap = {
      {schedule::gtfs::utils::GTFS_FILE_TYPE::AGENCY, agencyFile},
      {schedule::gtfs::utils::GTFS_FILE_TYPE::CALENDAR, calendarFile},
      {schedule::gtfs::utils::GTFS_FILE_TYPE::CALENDAR_DATES, calendarDatesFile},
      {schedule::gtfs::utils::GTFS_FILE_TYPE::ROUTES, routesFile},
      {schedule::gtfs::utils::GTFS_FILE_TYPE::STOP, stopFile},
      {schedule::gtfs::utils::GTFS_FILE_TYPE::STOP_TIMES, stopTimeFile},
      {schedule::gtfs::utils::GTFS_FILE_TYPE::TRANSFERS, TransferFile},
      {schedule::gtfs::utils::GTFS_FILE_TYPE::TRIPS, TripFile}};

    // request.departureStop = schedule::gtfs::Stop{"Stop1", "Mels", geometry::Coordinate{0.0},
    //                                              geometry::Coordinate{0.0}, "Par"};
    // request.departureTime = 1000;
    // request.arrivalStop = schedule::gtfs::Stop{"Stop2", "Sargans", geometry::Coordinate{0.0},
    //                                            geometry::Coordinate{0.0}, "Par"};
  }

  void TearDown() override {
    std::cout << "GtfsReaderStrategiesTest::TearDown()" << '\n';
    reader.reset(nullptr);
  }
};


TEST_F(RaptorTest, InitialTest) {


  const std::function<void(gtfs::GtfsReader&)> agencyStrategy = gtfs::GtfsAgencyReader(lFileNameMap.at(schedule::gtfs::utils::GTFS_FILE_TYPE::AGENCY));
  const std::function<void(gtfs::GtfsReader&)> calendarStrategy = gtfs::GtfsCalendarReader(lFileNameMap.at(schedule::gtfs::utils::GTFS_FILE_TYPE::CALENDAR));
  const std::function<void(gtfs::GtfsReader&)> calendarDatesStrategy = gtfs::GtfsCalendarDateReader(lFileNameMap.at(schedule::gtfs::utils::GTFS_FILE_TYPE::CALENDAR_DATES));
  const std::function<void(gtfs::GtfsReader&)> routesStrategy = gtfs::GtfsRouteReader(lFileNameMap.at(schedule::gtfs::utils::GTFS_FILE_TYPE::ROUTES));
  const std::function<void(gtfs::GtfsReader&)> stopStrategy = gtfs::GtfsStopReader(lFileNameMap.at(schedule::gtfs::utils::GTFS_FILE_TYPE::STOP));
  const std::function<void(gtfs::GtfsReader&)> stopTimeStrategy = gtfs::GtfsStopTimeReader(lFileNameMap.at(schedule::gtfs::utils::GTFS_FILE_TYPE::STOP_TIMES));
  const std::function<void(gtfs::GtfsReader&)> transferStrategy = gtfs::GtfsTransferReader(lFileNameMap.at(schedule::gtfs::utils::GTFS_FILE_TYPE::TRANSFERS));
  const std::function<void(gtfs::GtfsReader&)> tripStrategy = gtfs::GtfsTripReader(lFileNameMap.at(schedule::gtfs::utils::GTFS_FILE_TYPE::TRIPS));

  std::vector strategies = {agencyStrategy, calendarStrategy, calendarDatesStrategy, routesStrategy, stopStrategy, stopTimeStrategy, transferStrategy, tripStrategy}; //

  const std::unique_ptr<schedule::DataReader<schedule::DataContainer<schedule::gtfs::GtfsData>>> reader = std::make_unique<gtfs::GtfsReader>(std::move(strategies));
  reader->readData();
  const auto data = reader->getData();
  auto relationManager = schedule::gtfs::RelationManager(data.get());

  auto stopIdSource = relationManager.getStopIdFromName("Zürich, Bahnhof Affoltern");
  auto stopIdTarget = relationManager.getStopIdFromName("Erlenbach ZH, Bahnhof");

  // raptor::utils::ConnectionRequest request(stopIdSource, stopIdTarget, 1000);

  const std::unique_ptr<raptor::strategy::factory::IRaptorAlgorithmFactory> factory = std::make_unique<raptor::strategy::factory::RaptorAlgorithmFactory>();
  auto strategy = factory->create(raptor::strategy::factory::IRaptorAlgorithmFactory::RAPTOR, std::move(relationManager));

  try
  {
    std::unique_ptr<raptor::IRaptor> raptor = std::make_unique<raptor::Raptor>(std::move(strategy));
    //  auto connections = raptor->getConnections(request);
  } catch (const std::invalid_argument& e)
  {
    std::cout << e.what() << std::endl;
  }

}