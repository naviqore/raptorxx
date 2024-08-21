//
// Created by MichaelBrunner on 15/07/2024.
//

#pragma once

#include <algorithm>
#include <map>
#include <string>
#include <vector>
#include <ranges>
#include <csv2.hpp>

namespace schedule::gtfs::utils {



  /// @brief https://stackoverflow.com/a/2223926
  /// @param text string to remove BOM from
  /// @return
  inline std::string removeUtf8Bom(const std::string& text) {
    if (const auto utf8Bom = "\xEF\xBB\xBF";
        text.rfind(utf8Bom, 0) == 0)
    {                        // BOM is at the start of the string
      return text.substr(3); // Remove the first three bytes
    }
    return text; // Return the original string if BOM is not found
  }

  inline std::map<size_t, std::string> createHeaderMap(const std::vector<std::string>& headerItems) {
    std::map<size_t, std::string> headerMap;
    for (const auto& [index, value] : std::views::enumerate(headerItems))
    {
      auto columnName = value;
      columnName = removeUtf8Bom(columnName);
      columnName.erase(std::ranges::remove(columnName, '\r').begin(), columnName.end());
      headerMap[index] = columnName;
    }
    return headerMap;
  }

  inline std::vector<std::string> mapHeaderItemsToVector(const csv2::Reader<>::Row& header) {
    std::vector<std::string> headerItems;
    for (const auto& headerItem : header)
    {
      std::string value;
      headerItem.read_value(value);
      value = removeUtf8Bom(value);

      headerItems.push_back(value);
    }
    return headerItems;
  }
}
