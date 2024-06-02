//
// Created by MichaelBrunner on 01/06/2024.
//

#ifndef UTILS_H
#define UTILS_H

#include <chrono>
#include <string>
#include <vector>
#include <sstream>

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
    return fields;
  }

  inline std::chrono::year_month_day parseDate(const std::string& date_str) {
    std::istringstream ss(date_str);
    auto tm = std::tm{};
    ss >> std::get_time(&tm, "%Y%m%d");
    return std::chrono::year{tm.tm_year + YEAR_OFFSET} / (tm.tm_mon + gtfs::utils::MONTH_OFFSET) / tm.tm_mday;
  }

  enum class GTFS_FILE_TYPE
  {
    AGENCY,
    CALENDAR
  };


}



#endif //UTILS_H
