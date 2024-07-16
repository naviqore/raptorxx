//
// Created by MichaelBrunner on 15/07/2024.
//

#include "GtfsRouteReaderCsv.h"
#include "GtfsCsvHelpers.h"
#include <csv2/reader.hpp>
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

  GtfsRouteReaderCsv::GtfsRouteReaderCsv(std::string&& filename)
    : filename(std::move(filename)) {
    if (this->filename.empty())
    {
      throw std::invalid_argument("Filename is empty");
    }
  }
  GtfsRouteReaderCsv::GtfsRouteReaderCsv(std::string const& filename)
    : filename(filename) {
    if (this->filename.empty())
    {
      throw std::invalid_argument("Filename is empty");
    }
  }

  void GtfsRouteReaderCsv::operator()(GtfsReader& aReader) const {
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
      TempRoute tempRoute;
      index = 0;
      for (const auto& cell : row)
      {
        auto columnName = headerMap[index];

        std::string value;
        cell.read_value(value);
        value.erase(std::ranges::remove(value, '\r').begin(), value.end());
        value = utils::removeUtf8Bom(value);
        value = utils::removeQuotesFromStringView(value);

        static const std::map<std::string, std::function<void(TempRoute&, const std::string&)>> columnActions = {
          {"route_id", [](TempRoute& route, const std::string& val) { route.routeId = val; }},
          {"agency_id", [](TempRoute& route, const std::string& val) { route.agencyId = val; }},
          {"route_short_name", [](TempRoute& route, const std::string& val) { route.routeShortName = val; }},
          {"route_long_name", [](TempRoute& route, const std::string& val) { route.routeLongName = val; }},
          {"route_desc", [](TempRoute& route, const std::string& val) { route.routeDesc = val; }},
          {"route_type", [](TempRoute& route, const std::string& val) { route.routeType = val; }},

        };

        if (columnActions.contains(columnName))
        {
          columnActions.at(columnName)(tempRoute, value);
        }
        ++index;
      }
      if (!tempRoute.routeId.empty())
      {
        auto routeId = tempRoute.routeId;


        aReader.getData().get().routes.emplace(routeId,
                                               Route{
                                                 std::move(tempRoute.routeId),
                                                 std::move(tempRoute.routeShortName),
                                                 std::move(tempRoute.routeLongName),
                                                 static_cast<Route::RouteType>(std::stoi(tempRoute.routeType.empty() == false ? tempRoute.routeType : "99999")), // defined 99999 as undefined
                                                 std::move(tempRoute.agencyId),
                                               });
      }
    }
  }
}