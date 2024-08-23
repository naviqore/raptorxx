//
// Created by MichaelBrunner on 15/07/2024.
//

#include "GtfsStopReaderCsv.h"
#include "GtfsCsvHelpers.h"
#include "src/utils/utils.h"

#include <csv2.hpp>
#include <ranges>

namespace schedule::gtfs {

  struct TempStop
  {
    std::string stopId;
    std::string stopName;
    std::string stopLat;
    std::string stopLon;
    std::string parentStation;
  };

  GtfsStopReaderCsv::GtfsStopReaderCsv(std::string&& filename)
    : filename(std::move(filename)) {
    if (this->filename.empty())
    {
      throw std::invalid_argument("Filename is empty");
    }
  }
  GtfsStopReaderCsv::GtfsStopReaderCsv(std::string const& filename)
    : filename(filename) {
      if (this->filename.empty())
      {
        throw std::invalid_argument("Filename is empty");
      }
  }

  void GtfsStopReaderCsv::operator()(GtfsReader& aReader) const {
    auto reader = csv2::Reader();
    if (!reader.mmap(filename))
    {
      throw std::runtime_error("Cannot read file");
    }
    const auto header = reader.header();
    const std::vector<std::string> headerItems = utils::mapHeaderItemsToVector(header);

    std::map<size_t, std::string> headerMap = utils::createHeaderMap(headerItems);

    int index = 0;
    for (const auto& row : reader)
    {
      TempStop tempStop;
      index = 0;
      for (const auto& cell : row)
      {
        auto columnName = headerMap[index];

        std::string value;
        cell.read_value(value);
        value.erase(std::ranges::remove(value, '\r').begin(), value.end());
        value = utils::removeUtf8Bom(value);
        value = utils::removeQuotesFromStringView(value);

        static const std::map<std::string, std::function<void(TempStop&, const std::string&)>> columnActions = {
          {"stop_id", [](TempStop& stop, const std::string& val) { stop.stopId = val; }},
          {"stop_name", [](TempStop& stop, const std::string& val) { stop.stopName = val; }},
          {"stop_lat", [](TempStop& stop, const std::string& val) { stop.stopLat = val; }},
          {"stop_lon", [](TempStop& stop, const std::string& val) { stop.stopLon = val; }},
          {"parent_station", [](TempStop& stop, const std::string& val) { stop.parentStation = val; }},

        };

        if (columnActions.contains(columnName))
        {
          columnActions.at(columnName)(tempStop, value);
        }
        ++index;
      }
      if (!tempStop.stopId.empty())
      {
        auto stopId = tempStop.stopId;


        aReader.getData().get().stops.emplace(stopId,
                                              Stop{
                                                std::move(tempStop.stopId),
                                                std::move(tempStop.stopName),
                                                geometry::Coordinate<double>(std::stod(tempStop.stopLat)),
                                                geometry::Coordinate<double>(std::stod(tempStop.stopLon)),
                                                std::move(tempStop.parentStation)});
      }
    }
  }
}
