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
#include <map>

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

  inline std::string_view removeQuotesFromStringView(std::string_view field) {
    if (field.size() >= 2 && field.front() == '\"' && field.back() == '\"')
    {
      field.remove_prefix(1);
      field.remove_suffix(1);
    }
    return field;
  }

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
        field = removeQuotesFromStringView(field);
        result.push_back(field);
        start = end + 1;
      }
    }

    if (start < line.size())
    {
      std::string_view field = line.substr(start);
      field = removeQuotesFromStringView(field);
      result.push_back(field);
    }

    return result;
  }

  inline auto getGtfsColumnIndices(const std::string_view line) {
    std::map<std::string, size_t> headerMap;
    const std::vector<std::string_view> fieldsHeader = splitLineAndRemoveQuotes(line);
    for (size_t i = 0; i < fieldsHeader.size(); ++i)
    {
      headerMap[std::string(fieldsHeader[i])] = i;
    }
    return headerMap;
  }

}



#endif //UTILS_H
