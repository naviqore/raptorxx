//
// Created by MichaelBrunner on 15/07/2024.
//

#include "GtfsRouteReaderCsvParser.h"

#include "csv_wrapper.h"

#include <ranges>

namespace schedule::gtfs {
  struct TempRoute
  {
    std::string routeId;
    std::string agencyId;
    std::string routeShortName;
    std::string routeLongName;
    std::string routeDesc;
    std::string routeType;
  };

  GtfsRouteReaderCsvParser::GtfsRouteReaderCsvParser(std::string&& filename)
    : filename(std::move(filename)) {
    if (this->filename.empty())
    {
      throw std::invalid_argument("Filename is empty");
    }
  }
  GtfsRouteReaderCsvParser::GtfsRouteReaderCsvParser(std::string const& filename)
    : filename(filename) {
    if (this->filename.empty())
    {
      throw std::invalid_argument("Filename is empty");
    }
  }

  void GtfsRouteReaderCsvParser::operator()(GtfsReader& aReader) const {
    auto reader = io::CSVReader<6, io::trim_chars<'"'>, io::double_quote_escape<',', '\"'>>(filename); // , io::trim_chars<'"'>
    reader.set_header("route_id", "agency_id", "route_short_name", "route_long_name", "route_desc", "route_type");

    std::string routeId;
    std::string agencyId;
    std::string routeShortName;
    std::string routeLongName;
    std::string routeDesc;
    int routeType;
    reader.read_header(io::ignore_no_column, "route_id", "agency_id", "route_short_name", "route_long_name", "route_desc", "route_type");
    while (reader.read_row(routeId, agencyId, routeShortName, routeLongName, routeDesc, routeType))
    {
      std::ignore = routeDesc;
      auto temp = routeId;

      aReader.getData().get().routes.try_emplace(
        temp,
        Route{std::move(routeId),
              std::move(routeShortName),
              std::move(routeLongName),
              static_cast<Route::RouteType>(routeType), // TODO handle exception
              std::move(agencyId)});
    }
  }
}