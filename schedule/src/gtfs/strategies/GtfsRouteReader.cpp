//
// Created by MichaelBrunner on 07/06/2024.
//

#include "GtfsRouteReader.h"

#include "LoggingPool.h"
#include "gtfs/GtfsReader.h"
#include "src/utils/utils.h"
#include "utils/scopedTimer.h"

#include <fstream>

namespace gtfs {
  void GtfsRouteReader::operator()(GtfsReader& aReader) const {
    MEASURE_FUNCTION(std::source_location().file_name());
    std::ifstream infile(filename);
    if (!infile.is_open())
    {
      throw std::runtime_error("Error opening file: " + std::string(filename));
    }

    std::string line;
    std::getline(infile, line); // Skip header line
    std::vector<std::string_view> fields;
    fields.reserve(7);
    while (std::getline(infile, line))
    {
      fields = schedule::gtfs::utils::splitLineAndRemoveQuotes(line);
      if (fields.size() < 7)
      {
        // TODO: Handle error
        continue;
      }

#ifdef OPEN_DATA_ZURICH
      aReader.getData().get().routes.emplace_back(std::string(fields[0]),
                                                  std::string(fields[2]),
                                                  std::string(fields[3]),
                                                  static_cast<schedule::gtfs::Route::RouteType>(std::stoi(std::string(fields[4]))));
#else

      aReader.getData().get().routes.emplace_back(std::string(fields[0]),
                                                  std::string(fields[2]),
                                                  std::string(fields[3]),
                                                  static_cast<schedule::gtfs::Route::RouteType>(std::stoi(std::string(fields[5]))));
#endif
    }
  }

  GtfsRouteReader::GtfsRouteReader(std::string filename)
    : filename(std::move(filename)) {
  }
} // gtfs
