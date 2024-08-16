//
// Created by MichaelBrunner on 15/07/2024.
//

#include "GtfsStopTimeReaderCsv.h"
#include "GtfsCsvHelpers.h"
#include "src/utils/utils.h"

#include <csv2/reader.hpp>
#include <ranges>

namespace schedule::gtfs {

  struct TempStopTime
  {
    std::string stopId;
    std::string tripId;
    std::string arrivalTime;
    std::string departureTime;
    std::string stopSequence;
  };

  GtfsStopTimeReaderCsv::GtfsStopTimeReaderCsv(std::string&& filename)
    : filename(std::move(filename)) {
    if (this->filename.empty())
    {
      throw std::invalid_argument("Filename is empty");
    }
  }
  GtfsStopTimeReaderCsv::GtfsStopTimeReaderCsv(std::string const& filename)
    : filename(filename) {
    if (this->filename.empty())
    {
      throw std::invalid_argument("Filename is empty");
    }
  }

  void GtfsStopTimeReaderCsv::operator()(GtfsReader& aReader) const {
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
      TempStopTime tempStop;
      index = 0;
      for (const auto& cell : row)
      {
        auto columnName = headerMap[index];

        std::string value;
        cell.read_value(value);
        value.erase(std::ranges::remove(value, '\r').begin(), value.end());
        value = utils::removeUtf8Bom(value);
        value = utils::removeQuotesFromStringView(value);

        static const std::map<std::string, std::function<void(TempStopTime&, const std::string&)>> columnActions = {
          {"stop_id", [](TempStopTime& stop, const std::string& val) { stop.stopId = val; }},
          {"trip_id", [](TempStopTime& stop, const std::string& val) { stop.tripId = val; }},
          {"arrival_time", [](TempStopTime& stop, const std::string& val) { stop.arrivalTime = val; }},
          {"departure_time", [](TempStopTime& stop, const std::string& val) { stop.departureTime = val; }},
          {"stop_sequence", [](TempStopTime& stop, const std::string& val) { stop.stopSequence = val; }},
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

        aReader.getData().get().stopTimes[stopId].emplace_back(
          std::move(tempStop.tripId),
          std::move(tempStop.arrivalTime),
          std::move(tempStop.departureTime),
          std::move(tempStop.stopId),
          std::stoi(tempStop.stopSequence));
      }
    }
  }
}