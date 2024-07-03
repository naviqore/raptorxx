//
// Created by MichaelBrunner on 07/06/2024.
//

#include "GtfsRouteReader.h"

#include "LoggingPool.h"
#include "gtfs/GtfsReader.h"
#include "src/utils/utils.h"
#include "utils/scopedTimer.h"

#include <fstream>
#include <map>

namespace gtfs {
  void GtfsRouteReader::operator()(GtfsReader& aReader) const {
    MEASURE_FUNCTION(std::source_location().file_name());
    std::ifstream infile(filename);
    if (!infile.is_open())
    {
      throw std::runtime_error("Error opening file: " + std::string(filename));
    }
    LoggingPool::getInstance(Target::CONSOLE)->info(std::format("Reading file: {}", filename));
    std::string line;
    std::getline(infile, line); // Get Header line
    // get indices of header columns
    std::map<std::string, size_t> headerMap = schedule::gtfs::utils::getGtfsColumnIndices(line);

    std::vector<std::string_view> fields;
    fields.reserve(7);
    while (std::getline(infile, line))
    {
      fields = schedule::gtfs::utils::splitLineAndRemoveQuotes(line);
      if (fields.size() < 6)
      {
        // TODO: Handle error
        continue;
      }

      aReader.getData().get().routes.emplace_back(std::string(fields[headerMap["route_id"]]),
                                                  std::string(fields[headerMap["route_short_name"]]),
                                                  std::string(fields[headerMap["route_long_name"]]),
                                                  static_cast<schedule::gtfs::Route::RouteType>(std::stoi(std::string(fields[headerMap["route_type"]]))));
    }
  }

  GtfsRouteReader::GtfsRouteReader(std::string filename)
    : filename(std::move(filename)) {
  }
} // gtfs
