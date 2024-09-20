//
// Created by MichaelBrunner on 15/07/2024.
//

#include "GtfsTripReaderCsvParser.h"

#include "csv_wrapper.h"

#include <csv2.hpp>

namespace schedule::gtfs {

  struct TempTrip {
    std::string tripId;
    std::string routeId;
    std::string serviceId;
  };

  GtfsTripReaderCsvParser::GtfsTripReaderCsvParser(std::string&& filename)
    : filename(std::move(filename))
  {
    if (this->filename.empty()) {
      throw std::invalid_argument("Filename is empty");
    }
  }
  GtfsTripReaderCsvParser::GtfsTripReaderCsvParser(std::string const& filename)
    : filename(filename)
  {
    if (this->filename.empty()) {
      throw std::invalid_argument("Filename is empty");
    }
  }

  void GtfsTripReaderCsvParser::operator()(GtfsReader& aReader) const
  {
    auto reader = io::CSVReader<7, io::trim_chars<'"'>, io::double_quote_escape<',', '\"'>>(filename); // , io::trim_chars<'"'>
    reader.set_header("route_id", "service_id", "trip_id", "trip_headsign", "trip_short_name", "direction_id", "block_id");

    std::string routeId;
    std::string serviceId;
    std::string tripId;
    std::string tripHeadsign;
    std::string tripShortName;
    std::string directionId;
    std::string blockId;

    reader.read_header(io::ignore_extra_column, "route_id", "service_id", "trip_id", "trip_headsign", "trip_short_name", "direction_id", "block_id");

    while (reader.read_row(routeId, serviceId, tripId, tripHeadsign, tripShortName, directionId, blockId)) {
      std::ignore = tripHeadsign;
      std::ignore = tripShortName;
      std::ignore = directionId;
      std::ignore = blockId;

      auto temp = tripId;

      aReader.getData().get().trips.emplace(temp,
                                            std::make_shared<Trip>(std::move(routeId),
                                                                   std::move(serviceId),
                                                                   std::move(tripId)));
    }
  }
} // gtfs
  // schedule