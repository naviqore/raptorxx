//
// Created by MichaelBrunner on 01/06/2024.
//

#ifndef UTILS_H
#define UTILS_H

#include <chrono>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip> // std::get_time

namespace gtfs::utils {
  constexpr int YEAR_OFFSET = 1900;

  constexpr int MONTH_OFFSET = 1;

  inline std::vector<std::string> splitLine(const std::string& line, const char delimiter = ',') {
    std::vector<std::string> fields;
    std::istringstream lineStream(line);
    std::string field;
    while (std::getline(lineStream, field, delimiter))
    {
      fields.push_back(field);
    }
    // TODO think about returning an expected instead of a vector
    // auto result = std::unexpected("Error: insufficient number of fields.");
    return fields;
  }

  inline std::chrono::year_month_day parseDate(const std::string& date_str) {
    const int year = std::stoi(date_str.substr(0, 4));
    const int month = std::stoi(date_str.substr(4, 2));
    const int day = std::stoi(date_str.substr(6, 2));
    return std::chrono::year{year} / month / day;
  }

  enum class GTFS_FILE_TYPE
  {
    AGENCY,
    CALENDAR,
    CALENDAR_DATES,
    ROUTES,
    STOP,
    STOP_TIMES,
    TRANSFERS,
    TRIPS
  };


}



#endif //UTILS_H
