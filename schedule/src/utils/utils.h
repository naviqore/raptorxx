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

namespace schedule::gtfs::utils {
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

  inline std::vector<std::string_view> splitLineAndRemoveQuotes(const std::string_view line) {
    std::vector<std::string_view> result;
    size_t start = 0;
    size_t end = 0;
    bool inQuotes = false;

    for (size_t i = 0; i < line.size(); ++i)
    {
      if (line[i] == '\"')
      {
        inQuotes = !inQuotes;
      }
      else if (line[i] == ',' && !inQuotes)
      {
        end = i;
        std::string_view field = line.substr(start, end - start);

        // Remove quotes at the beginning and end of the field
        if (field.front() == '\"' && field.back() == '\"')
        {
          field.remove_prefix(1);
          field.remove_suffix(1);
        }

        result.push_back(field);
        start = end + 1;
      }
    }

    // Add the last field
    if (start < line.size())
    {
      std::string_view field = line.substr(start);
      if (field.front() == '\"' && field.back() == '\"')
      {
        field.remove_prefix(1);
        field.remove_suffix(1);
      }
      result.push_back(field);
    }

    return result;
  }

  /*inline std::vector<std::string_view> splitLineAndRemoveQuotes(const std::string_view line) {
    std::vector<std::string_view> result;
    size_t start = 0;
    size_t end = 0;
    bool inQuotes = false;

    for (size_t i = 0; i < line.size(); ++i)
    {
      if (line[i] == '\"')
      {
        inQuotes = !inQuotes;
      }
      else if (line[i] == ',' && !inQuotes)
      {
        end = i;
        if (start < end)
        {
          result.push_back(line.substr(start, end - start)); // Do not remove quotes
        }
        else
        {
          result.emplace_back(""); // push empty string when field is empty
        }
        start = end + 1;
      }
    }

    if (start < line.size())
    {
      result.push_back(line.substr(start, line.size() - start)); // Do not remove quotes
    }
    else
    {
      result.emplace_back(""); // push empty string when last field is empty
    }

    return result;
  }*/


}



#endif //UTILS_H
