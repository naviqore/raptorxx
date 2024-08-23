//
// Created by MichaelBrunner on 16/06/2024.
//

#ifndef SERVICEDAYTIME_H
#define SERVICEDAYTIME_H


#include <stdexcept>
#include <string>
#include <schedule_export.h>

namespace schedule::gtfs::utils {

  /// @brief std::chrono is way too slow for this purpose, so we use our own implementation
  class SCHEDULE_API ServiceDayTime
  {
    unsigned int totalSeconds = 0;

  public:
    class Second
    {
      unsigned int seconds = 0;

    public:
      explicit Second(const unsigned int seconds)
        : seconds(seconds) {
      }

      operator unsigned int() const {
        return seconds;
      }
    };

    class Minute
    {
      unsigned int minutes = 0;

    public:
      explicit Minute(const unsigned int minutes)
        : minutes(minutes) {
      }

      operator unsigned int() const {
        return minutes;
      }
    };

    class Hour
    {
      unsigned int hours = 0;

    public:
      explicit Hour(const unsigned int hours)
        : hours(hours) {
      }
      operator unsigned int() const {
        return hours;
      }
    };

    static constexpr unsigned int SECONDS_PER_MINUTE = 60;
    static constexpr unsigned int MINUTES_PER_HOUR = 60;
    static constexpr unsigned int HOURS_PER_DAY = 24;
    static constexpr unsigned int SECONDS_PER_HOUR = SECONDS_PER_MINUTE * MINUTES_PER_HOUR;
    static constexpr unsigned int SECONDS_PER_DAY = SECONDS_PER_HOUR * HOURS_PER_DAY;


    explicit ServiceDayTime(Second seconds);
    explicit ServiceDayTime(Hour hour, Minute minute, Second second);
    explicit ServiceDayTime(unsigned int hour, unsigned int minute, unsigned int second);

    ~ServiceDayTime() = default;
    ServiceDayTime(const ServiceDayTime&) = default;
    ServiceDayTime(ServiceDayTime&&) = default;
    ServiceDayTime& operator=(const ServiceDayTime&) = default;
    ServiceDayTime& operator=(ServiceDayTime&&) = default;

    [[nodiscard]] bool operator==(const ServiceDayTime& other) const;
    [[nodiscard]] bool operator!=(const ServiceDayTime& other) const;
    [[nodiscard]] bool operator<(const ServiceDayTime& other) const;
    [[nodiscard]] bool operator>(const ServiceDayTime& other) const;
    [[nodiscard]] bool operator<=(const ServiceDayTime& other) const;
    [[nodiscard]] bool operator>=(const ServiceDayTime& other) const;
    [[nodiscard]] operator bool() const;

    static ServiceDayTime fromString(std::string_view str);
    [[nodiscard]] std::string toString() const;

    [[nodiscard]] static ServiceDayTime fromSeconds(Second seconds);
    [[nodiscard]] static ServiceDayTime fromMinutes(Minute minutes);
    [[nodiscard]] static ServiceDayTime fromHours(Hour hours);
    [[nodiscard]] static ServiceDayTime fromDays(int days);

    [[nodiscard]] unsigned int toSeconds() const;
    [[nodiscard]] unsigned int toMinutes() const;
    [[nodiscard]] unsigned int toHours() const;
    [[nodiscard]] unsigned int toDays() const;
  };

} // utils
// gtfs

inline schedule::gtfs::utils::ServiceDayTime::Second operator"" _sec(const unsigned long long seconds) {
  return schedule::gtfs::utils::ServiceDayTime::Second(static_cast<unsigned int>(seconds));
}

#endif //SERVICEDAYTIME_H
