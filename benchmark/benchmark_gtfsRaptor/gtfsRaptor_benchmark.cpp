//
// Created by MichaelBrunner on 09/09/2024.
//

#include <benchmark/benchmark.h>
#include "DataReader.h"
#include "GtfsData.h"
#include "GtfsReader.h"
#include "GtfsReaderStrategyFactory.h"
#include "GtfsToRaptorConverter.h"
#include "LocalDateTime.h"
#include "LoggerFactory.h"
#include <DataContainer.h>
#include <numeric>

class GtfsRaptorFixture : public benchmark::Fixture {
protected:
  std::unique_ptr<schedule::DataReader<schedule::DataContainer<schedule::gtfs::GtfsData>>> reader;
  std::unique_ptr<schedule::gtfs::IGtfsReaderStrategyFactory> readerFactory;
  std::unique_ptr<raptor::RaptorRouter> raptorRouter;
  std::string basePath = TEST_DATA_DIR;
  schedule::gtfs::GtfsData data;

  raptor::utils::LocalDateTime time {raptor::utils::LocalDateTime{std::chrono::year{2024}, std::chrono::month{2}, std::chrono::day{1}, std::chrono::hours{0}, std::chrono::minutes{0}, std::chrono::seconds{0}}};

  raptor::config::QueryConfig queryConfig = {};

public:
  void SetUp(::benchmark::State& state) override
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

   // EIGHT_AM = START_OF_DAY.addHours(std::chrono::hours(8));
   // NINE_AM = START_OF_DAY.addHours(std::chrono::hours(9));
    // const auto dateTime = raptor::utils::LocalDateTime{std::chrono::year{2024}, std::chrono::month{2}, std::chrono::day{1}, std::chrono::hours{0}, std::chrono::minutes{0}, std::chrono::seconds{0}};
    auto mapper = converter::GtfsToRaptorConverter(std::move(data), 120, time);
    const auto raptor = mapper.convert();
    raptorRouter = std::make_unique<raptor::RaptorRouter>(std::move(*raptor));
  }

  void TearDown(::benchmark::State& state) override
  {
    reader.reset(nullptr);
  }
};

void BenchmarkRoute(benchmark::State& state, const std::string& fromStop, const std::string& toStop, const long long departureTime, const long long arrivalTime, const raptor::RaptorRouter& raptorRouter, const raptor::config::QueryConfig& queryConfig)
{
  for (auto _ : state) {
    std::ignore = raptorRouter.routeEarliestArrival(
      {{fromStop, static_cast<raptor::types::raptorInt>(departureTime)}},
      {{toStop, static_cast<raptor::types::raptorInt>(arrivalTime)}},
      queryConfig);
  }
}

// Benchmark function for route vonwilSG -> mels
BENCHMARK_F(GtfsRaptorFixture, BM_route_vonwilSG_mels)
(benchmark::State& state)
{
  // Route 1: vonwilSG -> mels
  BenchmarkRoute(state, "8589640", "8579885", time.secondsOfDay(), time.secondsOfDay() + 60 * 60 * 2, *raptorRouter, queryConfig);
}

BENCHMARK_F(GtfsRaptorFixture, BM_route_AbtwilDorf_Westcenter)
(benchmark::State& state)
{
  BenchmarkRoute(state, "8588889", "8589644", time.secondsOfDay(), time.secondsOfDay() + 60 * 60 * 2, *raptorRouter, queryConfig);
}

BENCHMARK_REGISTER_F(GtfsRaptorFixture, BM_route_vonwilSG_mels)->Iterations(10);
BENCHMARK_REGISTER_F(GtfsRaptorFixture, BM_route_AbtwilDorf_Westcenter)->Iterations(10);

int main(int argc, char** argv)
{
  ::benchmark::Initialize(&argc, argv);
  ::benchmark::RunSpecifiedBenchmarks();
  return 0;
}
