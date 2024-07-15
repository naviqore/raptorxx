//
// Created by MichaelBrunner on 14/07/2024.
//

#include "GtfsAgencyReaderCsv.h"

#include "GtfsCsvHelpers.h"

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
    if (this->filename.empty())
    {
      throw std::invalid_argument("Filename is empty");
    }
  }

  void GtfsAgencyReaderCsv::operator()(GtfsReader& aReader) const {
    auto reader = csv2::Reader();

    if (false == reader.mmap(filename))
    {
      throw std::runtime_error("can not read file");
    }
    const auto header = reader.header();
    const std::vector<std::string> headerItems = utils::mapHeaderItemsToVector(header);

    std::map<size_t, std::string> headerMap = utils::createHeaderMap(headerItems);

    int index = 0;
    for (const auto& row : reader)
    {
      TempAgency tempAgency;
      index = 0;
      for (const auto& cell : row)
      {
        auto columnName = headerMap[index];

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