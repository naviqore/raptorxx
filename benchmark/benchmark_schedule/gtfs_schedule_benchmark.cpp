//
// Created by MichaelBrunner on 16/06/2024.
//

#include <benchmark/benchmark.h>

#include <algorithm>
#include <functional>
#include <iostream>
#include <memory>
#include <ranges>
#include <vector>

#include "GtfsReaderStrategyFactory.h"
#include "include/GtfsReader.h"

static void BM_read_gtfs_schedule(benchmark::State& state) {
  auto strategy =
      std::vector<std::function<void(schedule::gtfs::GtfsReader&)>>();
  std::string basePath = TEST_DATA_DIR;

  const auto readerFactory = schedule::gtfs::createGtfsReaderStrategyFactory(
    schedule::gtfs::ReaderType::CSV_PARALLEL, std::move(basePath));

  const auto agencyStrategy =
      readerFactory->getStrategy(GtfsStrategyType::AGENCY);
  const auto calendarStrategy =
      readerFactory->getStrategy(GtfsStrategyType::CALENDAR);
  const auto calendarDatesStrategy =
      readerFactory->getStrategy(GtfsStrategyType::CALENDAR_DATE);
  const auto routesStrategy =
      readerFactory->getStrategy(GtfsStrategyType::ROUTE);
  const auto stopStrategy = readerFactory->getStrategy(GtfsStrategyType::STOP);
  const auto stopTimeStrategy =
      readerFactory->getStrategy(GtfsStrategyType::STOP_TIME);
  const auto transferStrategy =
      readerFactory->getStrategy(GtfsStrategyType::TRANSFER);
  const auto tripStrategy = readerFactory->getStrategy(GtfsStrategyType::TRIP);

  strategy.push_back(agencyStrategy);
  strategy.push_back(calendarStrategy);
  strategy.push_back(calendarDatesStrategy);
  strategy.push_back(routesStrategy);
  strategy.push_back(stopStrategy);
  strategy.push_back(stopTimeStrategy);
  strategy.push_back(transferStrategy);
  strategy.push_back(tripStrategy);

  const auto reader = std::make_unique<schedule::gtfs::GtfsReader>(std::move(strategy));
  for (auto _ : state) {
    reader->readData();
  }
}

BENCHMARK(BM_read_gtfs_schedule);

