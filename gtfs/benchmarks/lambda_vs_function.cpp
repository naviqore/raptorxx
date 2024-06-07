//
// Created by MichaelBrunner on 28/05/2024.
//

#include "GtfsReader.h"
#include "strategies/GtfsCalendarDateReader.h"
#include "GtfsData.h"
#include "model/Agency.h"


#include <benchmark/benchmark.h>
#include <vector>
#include <functional>
#include <iterator>
#include <algorithm>
#include <iostream>
#include <memory>
#include <ranges>


static void use_lambda(benchmark::State& state) {
  auto lambda = [](int v) { return v * 3; };
  using L = decltype(lambda);
  auto fs = std::vector<L>{};
  fs.resize(10'000'000, lambda);
  auto res = 1;
  for (auto _ : state)
  {
    for (const auto& f : fs)
    {
      benchmark::DoNotOptimize(f(res));
    }
  }
}

static void use_std_function(benchmark::State& state) {
  auto lbd = [](int v) { return v * 3; };
  using F = std::function<int(int)>;
  auto fs = std::vector<F>{};
  fs.resize(10'000'000, lbd);
  auto res = 1;
  for (auto _ : state)
  {
    for (const auto& f : fs)
    {
      benchmark::DoNotOptimize(f(res));
    }
  }
}

struct Student
{
  int year_{};
  int score_{};
  std::string name_{};
};

auto max_value(auto&& range) {
  const auto it = std::ranges::max_element(range);
  return it != range.end() ? *it : 0;
}

auto get_max_score(const std::vector<Student>& students, int year) {
  const auto by_year = [=](auto&& s) { return s.year_ == year; };
  return max_value(students
                   | std::views::filter(by_year)
                   | std::views::transform(&Student::score_));
}

static void BM_get_max_score(benchmark::State& state) {

  const std::vector<Student> students = {
    {2024, 90, "Alice"},
    {2024, 85, "Bob"},
    {2023, 95, "Charlie"},
  };

  for (auto _ : state)
  {
    for (auto il : {1, 2, 3, 4})
    {
      std::ignore = il;
      constexpr int year = 2024;

      benchmark::DoNotOptimize(get_max_score(students, year));
    }
  }
}

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
  std::string basePath = TEST_DATA_DIR_B;
  const std::function calendarDateStrategy = gtfs::GtfsCalendarDateReader(basePath + "calendar_dates.txt");
  strategy.push_back(calendarDateStrategy);
  std::unique_ptr<DataReader> reader = std::make_unique<gtfs::GtfsReader>(std::move(strategy));
  for (auto _ : state)
  {
    reader->readData();
  }
  auto s = benchmark::kSecond;
}

BENCHMARK(BM_read_calendar_dates);
BENCHMARK(BM_get_max_score);
BENCHMARK(use_lambda);
BENCHMARK(use_std_function);
BENCHMARK(BM_pointer);
BENCHMARK(BM_reference);

BENCHMARK_MAIN();