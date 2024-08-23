//
// Created by MichaelBrunner on 07/06/2024.
//

#include "GtfsTripReader.h"

#include "LoggerFactory.h"
#include "src/utils/utils.h"
#include "GtfsReader.h"
#include "utils/scopedTimer.h"

#include <fstream>

namespace schedule::gtfs {

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
    getLogger(Target::CONSOLE, LoggerName::GTFS)->info(std::format("Reading file: {}", filename));
    std::string line;
    std::getline(infile, line); // Skip header line
    std::map<std::string, size_t> headerMap = utils::getGtfsColumnIndices(line);

    std::vector<std::string_view> fields;
    fields.reserve(7);
    while (std::getline(infile, line))
    {
      fields = utils::splitLineAndRemoveQuotes(line);
      if (fields.size() < 6)
      {
        // TODO: Handle error
        continue;
      }
      aReader.getData().get().trips.emplace(std::string(fields[headerMap["trip_id"]]),
                                            Trip{std::string(fields[headerMap["route_id"]]),
                                                 std::string(fields[headerMap["service_id"]]),
                                                 std::string(fields[headerMap["trip_id"]])});
    }
  }
} // gtfs