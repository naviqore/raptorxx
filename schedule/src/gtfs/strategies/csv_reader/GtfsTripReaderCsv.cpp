//
// Created by MichaelBrunner on 15/07/2024.
//

#include "GtfsTripReaderCsv.h"

#include "GtfsCsvHelpers.h"
#include "src/utils/utils.h"

#include <csv2/reader.hpp>

namespace schedule::gtfs {

  struct TempTrip
  {
    std::string tripId;
    std::string routeId;
    std::string serviceId;
  };

  GtfsTripReaderCsv::GtfsTripReaderCsv(std::string&& filename)
    : filename(std::move(filename)) {
    if (this->filename.empty())
    {
      throw std::invalid_argument("Filename is empty");
    }
  }
  GtfsTripReaderCsv::GtfsTripReaderCsv(std::string const& filename)
    : filename(filename) {
    if (this->filename.empty())
    {
      throw std::invalid_argument("Filename is empty");
    }
  }

  void GtfsTripReaderCsv::operator()(GtfsReader& aReader) const {
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
      TempTrip tempTrip;
      index = 0;
      for (const auto& cell : row)
      {
        auto columnName = headerMap[index];

        std::string value;
        cell.read_value(value);
        value.erase(std::ranges::remove(value, '\r').begin(), value.end());
        value = utils::removeUtf8Bom(value);
        value = utils::removeQuotesFromStringView(value);

        static const std::map<std::string, std::function<void(TempTrip&, const std::string&)>> columnActions = {
          {"trip_id", [](TempTrip& transfer, const std::string& val) { transfer.tripId = val; }},
          {"route_id", [](TempTrip& transfer, const std::string& val) { transfer.routeId = val; }},
          {"service_id", [](TempTrip& transfer, const std::string& val) { transfer.serviceId = val; }},
        };

        if (columnActions.contains(columnName))
        {
          columnActions.at(columnName)(tempTrip, value);
        }
        ++index;
      }
      if (!tempTrip.tripId.empty())
      {
        auto tripId = tempTrip.tripId;

        aReader.getData().get().trips.emplace(tripId,
                                              Trip{std::move(tempTrip.routeId),
                                                   std::move(tempTrip.serviceId),
                                                   std::move(tempTrip.tripId)});
      }
    }
  }
} // gtfs
  // schedule