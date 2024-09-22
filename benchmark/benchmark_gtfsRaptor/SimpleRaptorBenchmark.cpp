//
// Created by MichaelBrunner on 21/09/2024.
//

#include "DataReader.h"
#include "GtfsData.h"
#include "GtfsReader.h"
#include "GtfsReaderStrategyFactory.h"
#include "GtfsToRaptorConverter.h"
#include "LocalDateTime.h"
#include <DataContainer.h>
#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

class SimpleRaptorBenchmark {
protected:
  static std::unique_ptr<schedule::DataReader<schedule::DataContainer<schedule::gtfs::GtfsData>>> reader;
  static std::unique_ptr<schedule::gtfs::IGtfsReaderStrategyFactory> readerFactory;
  static std::unique_ptr<raptor::RaptorRouter> raptorRouter;
  static schedule::gtfs::GtfsData data;
  static bool initialized;
  static raptor::utils::LocalDateTime time;
  static raptor::config::QueryConfig queryConfig;

public:
  static void setUp()
  {
    if (!initialized) {
      const std::string basePath = TEST_DATA_DIR;
      readerFactory = schedule::gtfs::createGtfsReaderStrategyFactory(schedule::gtfs::ReaderType::CSV_PARALLEL, basePath);

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
      data = reader->getData().get();

      const auto dateTime = raptor::utils::LocalDateTime{std::chrono::year{2024}, std::chrono::month{4}, std::chrono::day{26}, std::chrono::hours{8}, std::chrono::minutes{0}, std::chrono::seconds{0}};

      auto timetableManager = std::make_unique<converter::TimetableManager>(std::move(data), dateTime);
      auto mapper = converter::GtfsToRaptorConverter(120, std::move(timetableManager));
      const auto raptor = mapper.convert();
      raptorRouter = std::make_unique<raptor::RaptorRouter>(std::move(*raptor));
      initialized = true;
    }
  }

  static long long routeEarliestArrival(const std::string& fromStopId, const std::string& toStopId)
  {
    const auto startTime = std::chrono::high_resolution_clock::now();
    std::ignore = raptorRouter->routeEarliestArrival(
      {{fromStopId, static_cast<raptor::types::raptorInt>(time.secondsOfDay())}},
      {{toStopId, static_cast<raptor::types::raptorInt>(0)}},
      queryConfig);
    const auto endTime = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
  }

  static void benchmarkRoute(const std::string& fromStopId, const std::string& toStopId, const int iterations)
  {
    long long totalTime = 0;
    for (int i = 0; i < iterations; ++i) {
      totalTime += routeEarliestArrival(fromStopId, toStopId);
    }
    const long long averageTime = totalTime / iterations;
    std::cout << "Average time for routing from " << fromStopId << " to " << toStopId << " over " << iterations << " iterations: " << averageTime << " ms\n";
  }
};

bool SimpleRaptorBenchmark::initialized = false;
raptor::utils::LocalDateTime SimpleRaptorBenchmark::time{raptor::utils::LocalDateTime{std::chrono::year{2024}, std::chrono::month{1}, std::chrono::day{1}, std::chrono::hours{8}, std::chrono::minutes{0}, std::chrono::seconds{0}}};
std::unique_ptr<schedule::DataReader<schedule::DataContainer<schedule::gtfs::GtfsData>>> SimpleRaptorBenchmark::reader = nullptr;
std::unique_ptr<schedule::gtfs::IGtfsReaderStrategyFactory> SimpleRaptorBenchmark::readerFactory = nullptr;
std::unique_ptr<raptor::RaptorRouter> SimpleRaptorBenchmark::raptorRouter = nullptr;
schedule::gtfs::GtfsData SimpleRaptorBenchmark::data = {};
raptor::config::QueryConfig SimpleRaptorBenchmark::queryConfig = {};

int main(int argc, char** argv)
{
  SimpleRaptorBenchmark::setUp();
  // "8589640" "St. Gallen, Vonwil" to "8579885" "Mels, Bahnhof"
  SimpleRaptorBenchmark::benchmarkRoute("8589640", "8579885", 100);
  // "8574563","Maienfeld, Bahnhof" to "8587276" "Biel/Bienne, Taubenloch"
  SimpleRaptorBenchmark::benchmarkRoute("8574563", "8587276", 100);
  // "8588524","Sion, Hôpital Sud" to "8508896","Stans, Bahnhof"
  SimpleRaptorBenchmark::benchmarkRoute("8588524", "8508896", 100);
  // "8510709","Lugano, Via Domenico Fontana" to "8579255","Lausanne, Pont-de-Chailly"
  SimpleRaptorBenchmark::benchmarkRoute("8510709", "8579255", 100);
  // "8574848","Davos Dorf, Bahnhof" to "8576079","Rapperswil SG, Sonnenhof"
  SimpleRaptorBenchmark::benchmarkRoute("8574848", "8576079", 100);

  return 0;
}
