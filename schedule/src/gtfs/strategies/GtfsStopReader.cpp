//
// Created by MichaelBrunner on 07/06/2024.
//


#include "GtfsStopReader.h"
#include "LoggingPool.h"
#include "gtfs/GtfsReader.h"
#include "src/utils/utils.h"
#include "utils/scopedTimer.h"

#include <fstream>

namespace schedule::gtfs {

  void GtfsStopReader::operator()(GtfsReader& aReader) const {
    MEASURE_FUNCTION(std::source_location().file_name());
    std::ifstream infile(filename);
    if (!infile.is_open())
    {
      throw std::runtime_error("Error opening file: " + std::string(filename));
    }
    LoggingPool::getInstance().getLogger(Target::CONSOLE)->info(std::format("Reading file: {}", filename));
    std::string line;
    std::getline(infile, line); // Skip header line
    std::map<std::string, size_t> headerMap = utils::getGtfsColumnIndices(line);

    std::vector<std::string_view> fields;
    fields.reserve(6);
    while (std::getline(infile, line))
    {

      fields = utils::splitLineAndRemoveQuotes(line);
      if (fields.size() < 6) // due to different file format of the open data zurich dataset
      {
        continue;
      }

      aReader.getData().get().stops.emplace(
        std::string(fields[headerMap["stop_id"]]),
        Stop{
          std::string(fields[headerMap["stop_id"]]),
          std::string(fields[headerMap["stop_name"]]),
          geometry::Coordinate<double>(std::stod(std::string(fields[headerMap["stop_lat"]]))),
          geometry::Coordinate<double>(std::stod(std::string(fields[headerMap["stop_lon"]]))),
          std::string(fields[headerMap["parent_station"]])});
    }
  }

  GtfsStopReader::GtfsStopReader(std::string filename)
    : filename(std::move(filename)) {
  }
} // gtfs
