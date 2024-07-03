//
// Created by MichaelBrunner on 07/06/2024.
//

#include "GtfsTripReader.h"

#include "LoggingPool.h"
#include "src/utils/utils.h"
#include "gtfs/GtfsReader.h"
#include "utils/scopedTimer.h"

#include <fstream>

namespace gtfs {

  GtfsTripReader::GtfsTripReader(std::string filename)
    : filename(std::move(filename)) {
  }

  void GtfsTripReader::operator()(GtfsReader& aReader) const {
    MEASURE_FUNCTION(std::source_location().file_name());
    std::ifstream infile(filename);
    if (!infile.is_open())
    {
      throw std::runtime_error("Error opening file: " + std::string(filename));
    }
    LoggingPool::getInstance(Target::CONSOLE)->info(std::format("Reading file: {}", filename));
    std::string line;
    std::getline(infile, line); // Skip header line
    std::map<std::string, size_t> headerMap = schedule::gtfs::utils::getGtfsColumnIndices(line);

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
      aReader.getData().get().trips.emplace_back(std::string(fields[headerMap["route_id"]]),
                                                 std::string(fields[headerMap["service_id"]]),
                                                 std::string(fields[headerMap["trip_id"]]));
    }
  }
} // gtfs