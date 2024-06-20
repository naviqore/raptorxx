//
// Created by MichaelBrunner on 16/06/2024.
//

#include "gtfs/GtfsReader.h"
#include "gtfs/strategies/GtfsAgencyReader.h"
#include "gtfs/strategies/GtfsCalendarDateReader.h"
#include "gtfs/strategies/GtfsCalendarReader.h"
#include "gtfs/strategies/GtfsRouteReader.h"
#include "gtfs/strategies/GtfsStopReader.h"
#include "gtfs/strategies/GtfsStopTimeReader.h"
#include "gtfs/strategies/GtfsTransferReader.h"
#include "gtfs/strategies/GtfsTripReader.h"

#include <benchmark/benchmark.h>
#include <vector>
#include <functional>
#include <algorithm>
#include <iostream>
#include <memory>
#include <ranges>



static void BM_read_gtfs_schedule(benchmark::State& state) {
  auto strategy = std::vector<std::function<void(gtfs::GtfsReader&)>>();
  const std::string basePath = TEST_DATA_DIR;
  const std::function calendarDatesReaderStrategy = gtfs::GtfsCalendarDateReader(basePath + "calendar_dates.txt");
  const std::function calendarReaderStrategy = gtfs::GtfsCalendarReader(basePath + "calendar.txt");
  const std::function stopTimeReaderStrategy = gtfs::GtfsStopTimeReader(basePath + "stop_times.txt");
  const std::function agencyReaderStrategy = gtfs::GtfsAgencyReader(basePath + "agency.txt");
  const std::function routeReaderStrategy = gtfs::GtfsRouteReader(basePath + "routes.txt");
  const std::function stopReaderStrategy = gtfs::GtfsStopReader(basePath + "stops.txt");
  const std::function transferReaderStrategy = gtfs::GtfsTransferReader(basePath + "transfers.txt");
  const std::function tripReaderStrategy = gtfs::GtfsTripReader(basePath + "trips.txt");

  strategy.push_back(calendarDatesReaderStrategy);
  strategy.push_back(stopTimeReaderStrategy);
  strategy.push_back(agencyReaderStrategy);
  strategy.push_back(routeReaderStrategy);
  strategy.push_back(stopReaderStrategy);
  strategy.push_back(transferReaderStrategy);
  strategy.push_back(tripReaderStrategy);
  strategy.push_back(calendarReaderStrategy);

  const auto reader = std::make_unique<gtfs::GtfsReader>(std::move(strategy));
  for (auto _ : state)
  {
    reader->readData();
  }
}

BENCHMARK(BM_read_gtfs_schedule);

BENCHMARK_MAIN();