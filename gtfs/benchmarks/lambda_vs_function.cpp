//
// Created by MichaelBrunner on 28/05/2024.
//

#include <benchmark/benchmark.h>
#include <vector>
#include <functional>
#include <iterator>
#include <algorithm>
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

BENCHMARK(BM_get_max_score);
BENCHMARK(use_lambda);
BENCHMARK(use_std_function);

BENCHMARK_MAIN();