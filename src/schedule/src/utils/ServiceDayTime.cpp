//
// Created by MichaelBrunner on 16/06/2024.
//

#include "ServiceDayTime.h"

#include <chrono>
#include <stdexcept>


namespace schedule::gtfs::utils {
  ServiceDayTime::ServiceDayTime(const Second seconds)
    : totalSeconds(seconds) {
  }
  ServiceDayTime::ServiceDayTime(const Hour hour, const Minute minute, const Second second) {
    totalSeconds = hour * SECONDS_PER_HOUR + minute * SECONDS_PER_MINUTE + second;
  }
  ServiceDayTime::ServiceDayTime(unsigned int const hour, unsigned int const minute, unsigned int const second) {
    totalSeconds = hour * SECONDS_PER_HOUR + minute * SECONDS_PER_MINUTE + second;
  }

  bool ServiceDayTime::operator==(const ServiceDayTime& other) const {
    return totalSeconds == other.totalSeconds;
  }
  bool ServiceDayTime::operator!=(const ServiceDayTime& other) const {
    return totalSeconds != other.totalSeconds;
  }
  bool ServiceDayTime::operator<(const ServiceDayTime& other) const {
    return totalSeconds < other.totalSeconds;
  }
  bool ServiceDayTime::operator>(const ServiceDayTime& other) const {
    return totalSeconds > other.totalSeconds;
  }
  bool ServiceDayTime::operator<=(const ServiceDayTime& other) const {
    return totalSeconds <= other.totalSeconds;
  }
  bool ServiceDayTime::operator>=(const ServiceDayTime& other) const {
    return totalSeconds >= other.totalSeconds;
  }
  ServiceDayTime::operator bool() const {
    return totalSeconds != 0;
  }

  ServiceDayTime ServiceDayTime::fromString(std::string_view timeString) {
    size_t start = 0;
    size_t end = timeString.find(':');
    if (end == std::string_view::npos)
    {
      throw std::invalid_argument("Invalid time format");
    }
    const unsigned int hour = std::stoi(std::string(timeString.substr(start, end - start)));

    start = end + 1;
    end = timeString.find(':', start);
    if (end == std::string_view::npos)
    {
      throw std::invalid_argument("Invalid time format");
    }
    const unsigned int minute = std::stoi(std::string(timeString.substr(start, end - start)));

    start = end + 1;
    const unsigned int second = std::stoi(std::string(timeString.substr(start)));

    return ServiceDayTime(Hour(hour), Minute(minute), Second(second));
  }

  // std::string ServiceDayTime::toString() const {
  //   const unsigned int hour = totalSeconds / SECONDS_PER_HOUR;
  //   const unsigned int minute = (totalSeconds % SECONDS_PER_HOUR) / SECONDS_PER_MINUTE;
  //   const unsigned int second = totalSeconds % SECONDS_PER_MINUTE;
  //   return std::string(std::to_string(hour) + ":" + std::to_string(minute) + ":" + std::to_string(second));
  // }

  std::string ServiceDayTime::toString() const {
    auto seconds = std::chrono::seconds(totalSeconds);
    const auto hours = std::chrono::duration_cast<std::chrono::hours>(seconds);
    seconds -= hours; // remove hours from seconds
    const auto minutes = std::chrono::duration_cast<std::chrono::minutes>(seconds);
    seconds -= minutes; // remove minutes from seconds

    std::ostringstream stream;
    stream << std::setw(2) << std::setfill('0') << hours.count() << ":"
           << std::setw(2) << std::setfill('0') << minutes.count() << ":"
           << std::setw(2) << std::setfill('0') << seconds.count();

    return stream.str();
  }

  ServiceDayTime ServiceDayTime::fromSeconds(Second seconds) {
    return ServiceDayTime(seconds);
  }

  ServiceDayTime ServiceDayTime::fromMinutes(Minute minutes) {
    return ServiceDayTime(Second(minutes * SECONDS_PER_MINUTE));
  }

  ServiceDayTime ServiceDayTime::fromHours(Hour hours) {
    return ServiceDayTime(Second(hours * SECONDS_PER_HOUR));
  }

  ServiceDayTime ServiceDayTime::fromDays(int const days) {
    return ServiceDayTime(Second(days * SECONDS_PER_DAY));
  }

  unsigned int ServiceDayTime::toSeconds() const {
    return totalSeconds;
  }

  unsigned int ServiceDayTime::toMinutes() const {
    return totalSeconds / SECONDS_PER_MINUTE;
  }

  unsigned int ServiceDayTime::toHours() const {
    return totalSeconds / SECONDS_PER_HOUR;
  }

  unsigned int ServiceDayTime::toDays() const {
    return totalSeconds / SECONDS_PER_DAY;
  }
} // utils
// gtfs
