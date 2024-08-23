//
// Created by MichaelBrunner on 15/07/2024.
//

#include "GtfsStopReaderCsvParser.h"

#include "LoggerFactory.h"
#include "csv_wrapper.h"

#include <ranges>

namespace schedule::gtfs {

  GtfsStopReaderCsvParser::GtfsStopReaderCsvParser(std::string&& filename)
    : filename(std::move(filename)) {
    if (this->filename.empty())
    {
      throw std::invalid_argument("Filename is empty");
    }
  }
  GtfsStopReaderCsvParser::GtfsStopReaderCsvParser(std::string const& filename)
    : filename(filename) {
    if (this->filename.empty())
    {
      throw std::invalid_argument("Filename is empty");
    }
  }

  void GtfsStopReaderCsvParser::operator()(GtfsReader& aReader) const {
    auto reader = io::CSVReader<6, io::trim_chars<'"'>, io::double_quote_escape<',', '\"'>>(filename); // , io::trim_chars<'"'>
    reader.set_header("stop_id", "stop_name", "stop_lat", "stop_lon", "location_type", "parent_station");

    std::string stopId;
    std::string stopName;
    double stopLat;
    double stopLon;
    std::string locationType;
    std::string parentStation;
    reader.read_header(io::ignore_extra_column, "stop_id", "stop_name", "stop_lat", "stop_lon", "location_type", "parent_station");
    while (reader.read_row(stopId, stopName, stopLat, stopLon, locationType, parentStation))
    {
      std::ignore = locationType;
      auto temp = stopId;


      aReader.getData().get().stops.try_emplace(
        temp,
        Stop{std::move(stopId),
             std::move(stopName),
             geometry::Coordinate<double>((stopLat)),
             geometry::Coordinate<double>((stopLon)),
             std::move(parentStation)});
    }
  }

}