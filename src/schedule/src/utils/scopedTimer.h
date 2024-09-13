//
// Created by MichaelBrunner on 07/06/2024.
//

#ifndef SCOPEDTIMER_H
#define SCOPEDTIMER_H

#include <chrono>
#include <iostream>
#include <source_location>

#define USE_TIMER 1

#if USE_TIMER
#define MEASURE_FUNCTION()          \
  ScopedTimer timer                 \
  {                                 \
    std::source_location::current() \
  }
#else
#define MEASURE_FUNCTION()
#endif

class ScopedTimer {

public:
  using ClockType = std::chrono::steady_clock;

  explicit ScopedTimer(const std::source_location& location = std::source_location::current())
    : functionName{std::format("{}", location.function_name())}
    , start_{ClockType::now()}
  {
  }

  ScopedTimer(const ScopedTimer&) = delete;
  ScopedTimer(ScopedTimer&&) = delete;
  auto operator=(const ScopedTimer&) -> ScopedTimer& = delete;
  auto operator=(ScopedTimer&&) -> ScopedTimer& = delete;

  ~ScopedTimer()
  {
    using namespace std::chrono;
    const auto stop = ClockType::now();
    const auto duration = (stop - start_);
    const auto ms = duration_cast<milliseconds>(duration).count();
    std::cout << ms << " ms " << this->functionName << '\n';
  }

private:
  std::string functionName = {};
  const ClockType::time_point start_ = {};
};

inline auto some_function()
{
  MEASURE_FUNCTION();
  std::cout << "Do some work..." << '\n';
}

#endif //SCOPEDTIMER_H
