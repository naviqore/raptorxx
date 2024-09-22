//
// Created by MichaelBrunner on 04/08/2024.
//

#pragma once

#include <chrono>
#include <raptor_export.h>

using namespace std::chrono_literals;

namespace raptor::utils {

  class RAPTOR_API LocalDateTime {
    std::chrono::local_time<std::chrono::seconds> dateTime;

  public:
    LocalDateTime(const std::chrono::year year, const std::chrono::month month, const std::chrono::day day, const std::chrono::hours hour, const std::chrono::minutes minute, const std::chrono::seconds second)
    {
      using namespace std::chrono;
      dateTime = local_days{year / month / day} + hour + minute + second;
    }

    LocalDateTime() = default;

    [[nodiscard]] long long secondsOfDay() const
    {
      using namespace std::chrono;
      const auto time_since_epoch = dateTime.time_since_epoch();
      const auto days_since_epoch = duration_cast<days>(time_since_epoch);
      const auto time_of_day = time_since_epoch - days_since_epoch;
      return duration_cast<seconds>(time_of_day).count();
    }

    [[nodiscard]] long long getMinutes() const
    {
      return secondsOfDay() % 3600 / 60;
    }

    [[nodiscard]] long long getHours() const
    {
      return secondsOfDay() / 3600;
    }

    [[nodiscard]] long long getSeconds() const
    {
      return secondsOfDay() % 60;
    }

    [[nodiscard]] LocalDateTime addHours(const std::chrono::hours hours) const
    {
      using namespace std::chrono;
      const auto new_dateTime = dateTime + hours;
      const auto time_since_epoch = new_dateTime.time_since_epoch();
      const auto days_since_epoch = duration_cast<days>(time_since_epoch);
      const auto time_of_day = time_since_epoch - days_since_epoch;
      const auto new_hours = duration_cast<std::chrono::hours>(time_of_day);
      const auto new_minutes = duration_cast<minutes>(time_of_day % std::chrono::hours(1));
      const auto new_seconds = duration_cast<seconds>(time_of_day % minutes(1));
      return LocalDateTime{year{1970}, month{1}, day{1}, new_hours, new_minutes, new_seconds};
    }

    [[nodiscard]] std::chrono::year_month_day toYearMonthDay() const
    {
      using namespace std::chrono;
      const auto days_since_epoch = std::chrono::floor<days>(dateTime.time_since_epoch());
      const auto sys_days_since_epoch = sys_days{days_since_epoch};
      return year_month_day{sys_days_since_epoch};
    }
  };

  inline LocalDateTime fromSecondsOfDay(const long long total_seconds, const std::chrono::year year, const std::chrono::month month, const std::chrono::day day)
  {
    using namespace std::chrono;
    const auto hours = total_seconds / 3600;
    const auto minutes = (total_seconds % 3600) / 60;
    const auto seconds = total_seconds % 60;
    return LocalDateTime{year, month, day, std::chrono::hours(hours), std::chrono::minutes(minutes), std::chrono::seconds(seconds)};
  }

  inline LocalDateTime fromSecondsOfToday(const long long total_seconds)
  {
    using namespace std::chrono;
    const auto hours = total_seconds / 3600;
    const auto minutes = (total_seconds % 3600) / 60;
    const auto seconds = total_seconds % 60;

    const auto now = system_clock::now();

    const auto nowLocal = zoned_time{current_zone(), now}.get_local_time();
    // Convert to year_month_day
    const auto yearMonthDay = year_month_day{floor<days>(nowLocal)};

    return LocalDateTime{yearMonthDay.year(), yearMonthDay.month(), yearMonthDay.day(), std::chrono::hours(hours), std::chrono::minutes(minutes), std::chrono::seconds(seconds)};
  }


} // utils
// raptor
