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
#define MEASURE_FUNCTION(msg)                                                     \
ScopedTimer timer { msg }
#else
#define MEASURE_FUNCTION(msg)
#endif

class ScopedTimer {

public:
  using ClockType = std::chrono::steady_clock;

  explicit ScopedTimer(const char* func) : function_{func}, start_{ClockType::now()} {}

  ScopedTimer(const ScopedTimer&) = delete;
  ScopedTimer(ScopedTimer&&) = delete;
  auto operator=(const ScopedTimer&) -> ScopedTimer& = delete;
  auto operator=(ScopedTimer &&) -> ScopedTimer& = delete;

  ~ScopedTimer() {
    using namespace std::chrono;
    const auto stop = ClockType::now();
    const auto duration = (stop - start_);
    const auto ms = duration_cast<milliseconds>(duration).count();
    std::cout << ms << " ms " << function_ << '\n';
  }

private:
  const char* function_ = {};
  const ClockType::time_point start_ = {};
};

inline auto some_function() {
  MEASURE_FUNCTION(std::source_location().file_name());
  std::cout << "Do some work..." << '\n';
}

#endif //SCOPEDTIMER_H
