//
// Created by MichaelBrunner on 07/06/2024.
//

#include "GtfsRouteReader.h"

#include "LoggerFactory.h"
#include "GtfsReader.h"
#include "src/utils/utils.h"
#include "utils/scopedTimer.h"

#include <fstream>
#include <map>

namespace schedule::gtfs {
  void GtfsRouteReader::operator()(GtfsReader& aReader) const
  {
    MEASURE_FUNCTION();
    std::ifstream infile(filename);
    if (!infile.is_open()) {
      throw std::runtime_error("Error opening file: " + std::string(filename));
    }
    getLogger(Target::CONSOLE, LoggerName::GTFS)->info(std::format("Reading file: {}", filename));
    std::string line;
    std::getline(infile, line); // Get Header line
    // get indices of header columns
    std::map<std::string, size_t> headerMap = utils::getGtfsColumnIndices(line);

    std::vector<std::string_view> fields;
    fields.reserve(7);
    while (std::getline(infile, line)) {
      fields = utils::splitLineAndRemoveQuotes(line);


      aReader.getData().get().routes.emplace(std::string(fields[headerMap["route_id"]]),
                                             std::make_shared<Route>(std::string(fields[headerMap["route_id"]]),
                                                                     std::string(fields[headerMap["route_short_name"]]),
                                                                     std::string(fields[headerMap["route_long_name"]]),
                                                                     static_cast<Route::RouteType>(std::stoi(std::string(fields[headerMap["route_type"]]))),
                                                                     std::string(fields[headerMap["agency_id"]])));
    }
  }

  GtfsRouteReader::GtfsRouteReader(std::string filename)
    : filename(std::move(filename))
  {
  }
} // gtfs
