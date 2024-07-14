//
// Created by MichaelBrunner on 14/07/2024.
//



#include "gtfs/strategies/csv_reader/GtfsAgencyReaderCsv.h"


#include <gtest/gtest.h>

TEST(GTFS, TestAgencyReaderCsv) {
  const std::string basePath = TEST_DATA_DIR;
  auto agencyReader = schedule::gtfs::GtfsAgencyReaderCsv(std::move(basePath  + "agency.txt"));
  auto strategies = std::vector<std::function<void(schedule::gtfs::GtfsReader&)>>{agencyReader};
  const auto reader = std::make_unique<schedule::gtfs::GtfsReader>(std::move(strategies));
  reader->readData();
}