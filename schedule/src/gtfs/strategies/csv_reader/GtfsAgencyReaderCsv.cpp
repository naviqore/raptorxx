//
// Created by MichaelBrunner on 14/07/2024.
//

#include "GtfsAgencyReaderCsv.h"
#include "LoggerFactory.h"

#include <csv2/reader.hpp>

#include <utility>
#include <algorithm>
#include <ranges>


namespace schedule::gtfs {

  struct TempAgency
  {
    std::string id;
    std::string name;
    std::string timezone;
    std::string phone;
  };

  GtfsAgencyReaderCsv::GtfsAgencyReaderCsv(std::string&& filename)
    : filename(std::move(filename)) {
    if (filename.empty())
    {
      throw std::invalid_argument("Filename is empty");
    }
  }

  void GtfsAgencyReaderCsv::operator()(GtfsReader& aReader) const {
    auto reader = csv2::Reader<csv2::delimiter<','>,
                               csv2::quote_character<'"'>,
                               csv2::first_row_is_header<true>,
                               csv2::trim_policy::trim_whitespace>();

    if (false == reader.mmap(filename))
    {
      throw std::runtime_error("can not read file");
    }
    const auto header = reader.header();
    std::vector<std::string> headerItems;
    for (const auto& headerItem : header)
    {
      std::string value;
      headerItem.read_value(value);
      headerItems.push_back(value);
    }

    std::map<size_t, std::string> headerMap;
    std::map<std::string, std::function<void(std::string const&)>> headerTypes;
    for (const auto& [index, value] : std::views::enumerate(headerItems))
    {
      headerMap[index] = value;
    }

    auto index = 0;
    for (const auto& row : reader)
    {
      TempAgency tempAgency;
      index = 0;
      for (const auto& cell : row)
      {
        auto columnName = headerMap[index];
        columnName.erase(std::ranges::remove(columnName, '\r').begin(), columnName.end());
        std::string value;
        cell.read_value(value);
        value.erase(std::ranges::remove(value, '\r').begin(), value.end());

        static const std::map<std::string, std::function<void(TempAgency&, const std::string&)>> columnActions = {
          {"agency_id", [](TempAgency& agency, const std::string& val) { agency.id = val; }},
          {"agency_name", [](TempAgency& agency, const std::string& val) { agency.name = val; }},
          {"agency_timezone", [](TempAgency& agency, const std::string& val) { agency.timezone = val; }},
          {"agency_phone", [](TempAgency& agency, const std::string& val) { agency.phone = val; }}};

        if (columnActions.contains(columnName))
        {
          columnActions.at(columnName)(tempAgency, value);
        }

        ++index;
      }
      if (!tempAgency.name.empty())
      {
        auto tempName = tempAgency.name;
        aReader.getData().get().agencies.try_emplace(tempName,
                                                     Agency{std::move(tempAgency.id),
                                                            std::move(tempAgency.name),
                                                            std::move(tempAgency.timezone)});
      }
    }
  }


}
// gtfs
// schedule