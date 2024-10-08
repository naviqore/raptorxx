//
// Created by MichaelBrunner on 28/05/2024.
//

#include <GtfsReader.h>
#include <benchmark/benchmark.h>
#include <include/GtfsData.h>
#include <model/Agency.h>

#include <algorithm>
#include <functional>
#include <iostream>
#include <memory>
#include <ranges>
#include <vector>

#include "GtfsReaderStrategyFactory.h"

static void BM_pointer(benchmark::State& state)
{
  auto vec = std::vector<int*>();

  vec.resize(10'000, new int(12));

  auto printValue = [](const int* value) { return *value; };

  for (auto _ : state) {
    for (const auto& f : vec) {
      benchmark::DoNotOptimize(printValue(f));
    }
  }
}

static void BM_reference(benchmark::State& state)
{
  const auto ref = 12;
  auto vec = std::vector<int>();
  vec.resize(10'000, ref);
  auto printValue = [](const auto value) { return value; };

  for (auto _ : state) {
    for (const auto& f : vec) {
      benchmark::DoNotOptimize(printValue(f));
    }
  }
}

struct Record {
  std::string agencyId;
  std::string agencyName;
  std::string agencyUrl;
  std::string agencyTimezone;
  std::string agencyLang;
  std::string agencyPhone;
};

static void BM_rawPointer(benchmark::State& state)
{
  auto vec = std::vector<Record*>();

  vec.resize(10'000, new Record{"agencyId", "agencyName", "agencyUrl", "agencyTimezone", "agencyLang", "agencyPhone"});

  auto printValue = [](const Record* value) { return value->agencyId; };

  for (auto _ : state) {
    for (const auto f : vec) {
      benchmark::DoNotOptimize(printValue(f));
    }
  }
}

static void BM_uniquePointer(benchmark::State& state)
{
  auto vec = std::vector<std::unique_ptr<Record>>();

  vec.resize(10'000);
  for (auto& f : vec) {
    f = std::make_unique<Record>("agencyId", "agencyName", "agencyUrl", "agencyTimezone", "agencyLang", "agencyPhone");
  }

  auto printValue = [](const Record* value) { return value->agencyId; };

  for (auto _ : state) {
    for (const auto& f : vec) {
      benchmark::DoNotOptimize(printValue(f.get()));
    }
  }
}

class GtfsReaderFixture : public benchmark::Fixture {
protected:
  std::unique_ptr<schedule::gtfs::IGtfsReaderStrategyFactory> readerFactory;

public:
  void SetUp(::benchmark::State& state) override
  {
    readerFactory = schedule::gtfs::createGtfsReaderStrategyFactory(
      schedule::gtfs::ReaderType::CSV_PARALLEL, TEST_DATA_DIR);
  }

  void TearDown(::benchmark::State& state) override {}
};

BENCHMARK_F(GtfsReaderFixture, BM_read_agencies)
(benchmark::State& state)
{
  auto strategy = std::vector<std::function<void(schedule::gtfs::GtfsReader&)>>{};
  const std::function<void(schedule::gtfs::GtfsReader&)> readerStrategy = readerFactory->getStrategy(GtfsStrategyType::AGENCY);
  strategy.push_back(readerStrategy);
  const auto reader = std::make_unique<schedule::gtfs::GtfsReader>(std::move(strategy));
  for (auto _ : state) {
    reader->readData();
  }
}

BENCHMARK_F(GtfsReaderFixture, BM_read_calendar_dates)
(benchmark::State& state)
{
  auto strategy = std::vector<std::function<void(schedule::gtfs::GtfsReader&)>>();
  const std::function<void(schedule::gtfs::GtfsReader&)> readerStrategy = readerFactory->getStrategy(GtfsStrategyType::CALENDAR_DATE);
  strategy.push_back(readerStrategy);
  const auto reader = std::make_unique<schedule::gtfs::GtfsReader>(std::move(strategy));
  for (auto _ : state) {
    reader->readData();
  }
}

BENCHMARK_F(GtfsReaderFixture, BM_read_stop_times)
(benchmark::State& state)
{
  auto strategy = std::vector<std::function<void(schedule::gtfs::GtfsReader&)>>();
  const std::function<void(schedule::gtfs::GtfsReader&)> readerStrategy = readerFactory->getStrategy(GtfsStrategyType::STOP_TIME);
  strategy.push_back(readerStrategy);
  const auto reader = std::make_unique<schedule::gtfs::GtfsReader>(std::move(strategy));
  for (auto _ : state) {
    reader->readData();
  }
}

BENCHMARK_REGISTER_F(GtfsReaderFixture, BM_read_agencies);
BENCHMARK_REGISTER_F(GtfsReaderFixture, BM_read_calendar_dates);
BENCHMARK_REGISTER_F(GtfsReaderFixture, BM_read_stop_times);
BENCHMARK(BM_pointer);
BENCHMARK(BM_reference);
BENCHMARK(BM_rawPointer);
BENCHMARK(BM_uniquePointer);

BENCHMARK_MAIN();


