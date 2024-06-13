//
// Created by MichaelBrunner on 28/05/2024.
//

#include "gtfs/GtfsReader.h"
#include "gtfs/strategies/GtfsCalendarDateReader.h"
#include "gtfs/GtfsData.h"
#include "model/Agency.h"

#include <benchmark/benchmark.h>
#include <vector>
#include <functional>
#include <algorithm>
#include <iostream>
#include <memory>
#include <ranges>


static void BM_pointer(benchmark::State& state) {
  // auto ptr = std::make_shared<int>(12);
  auto vec = std::vector<int*>();

  vec.resize(10'000, new int(12));

  auto printValue = [](const int* value) { return *value; };

  for (auto _ : state)
  {
    for (const auto& f : vec)
    {
      benchmark::DoNotOptimize(printValue(f));
    }
  }
}

static void BM_reference(benchmark::State& state) {
  const auto ref = 12;
  auto vec = std::vector<int>();
  vec.resize(10'000, ref);
  auto printValue = [](const auto value) { return value; };

  for (auto _ : state)
  {
    for (const auto& f : vec)
    {
      benchmark::DoNotOptimize(printValue(f));
    }
  }
}

static void BM_read_calendar_dates(benchmark::State& state) {
  auto strategy = std::vector<std::function<void(gtfs::GtfsReader&)>>();
  const std::string basePath = TEST_DATA_DIR;
  const std::function calendarDateStrategy = gtfs::GtfsCalendarDateReader(basePath + "calendar_dates.txt");
  strategy.push_back(calendarDateStrategy);
  const std::unique_ptr<schedule::DataReader<schedule::DataContainer<gtfs::GtfsData>>> reader = std::make_unique<gtfs::GtfsReader>(std::move(strategy));
  for (auto _ : state)
  {
    reader->readData();
  }
}

BENCHMARK(BM_read_calendar_dates);
BENCHMARK(BM_pointer);
BENCHMARK(BM_reference);

BENCHMARK_MAIN();