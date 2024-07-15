//
// Created by MichaelBrunner on 15/07/2024.
//

#pragma once

#include <algorithm>
#include <map>
#include <string>
#include <vector>
#include <ranges>
#include <csv2/reader.hpp>

namespace schedule::gtfs::utils {

  inline std::map<size_t, std::string> createHeaderMap(const std::vector<std::string>& headerItems) {
    std::map<size_t, std::string> headerMap;
    for (const auto& [index, value] : std::views::enumerate(headerItems))
    {
      auto columnName = value;
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
      headerItems.push_back(value);
    }
    return headerItems;
  }
}
