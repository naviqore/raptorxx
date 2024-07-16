//
// Created by MichaelBrunner on 14/07/2024.
//

#ifndef DATATYPECONVERTER_H
#define DATATYPECONVERTER_H
#include "ServiceDayTime.h"
#include "model/CalendarDate.h"


#include <chrono>
#include <string>

namespace schedule::utils {

  template<typename T>
  T convert(std::string const& string);

  template<typename T>
  concept IsIntegral = std::is_integral_v<T>;

  template<typename T>
  concept IsFloating = std::is_floating_point_v<T>;

  template<IsIntegral T>
  T convert(std::string const& string) {
    return std::stoi(string);
  }

  template<IsFloating T>
  T convert(std::string const& string) {
    return std::stod(string);
  }

  template<typename T>
  concept HasFromString = requires(std::string_view sv) {
    { T::fromString(sv) } -> std::same_as<T>;
  };

  template<>
  inline gtfs::utils::ServiceDayTime convert(std::string const& string) {
    return gtfs::utils::ServiceDayTime::fromString(string);
  }

  template<>
  inline gtfs::CalendarDate::ExceptionType convert(std::string const& string) {
    if (string == "1")
    {
      return gtfs::CalendarDate::ExceptionType::SERVICE_ADDED;
    }
    if (string == "2")
    {
      return gtfs::CalendarDate::ExceptionType::SERVICE_REMOVED;
    }
    throw std::invalid_argument("Invalid exception type");
  }

  template<>
  inline std::string convert(std::string const& string) {
    return string;
  }

  template<typename T>
  T convertTo(std::string const& string) {
    if (string.size() > 2 && string.front() == '"' && string.back() == '"')
    {
      const auto trimmedString = string.substr(1, string.size() - 2);
      return convert<T>(trimmedString);
    }
    return convert<T>(string);
  }

}

#endif //DATATYPECONVERTER_H
