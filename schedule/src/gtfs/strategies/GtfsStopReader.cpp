//
// Created by MichaelBrunner on 07/06/2024.
//


#include "GtfsStopReader.h"
#include "LoggingPool.h"
#include "gtfs/GtfsReader.h"
#include "src/utils/utils.h"
#include "utils/scopedTimer.h"

#include <fstream>

namespace gtfs {

  void GtfsStopReader::operator()(GtfsReader& aReader) const {
    MEASURE_FUNCTION(std::source_location().file_name());
    std::ifstream infile(filename);
    if (!infile.is_open())
    {
      throw std::runtime_error("Error opening file: " + std::string(filename));
    }

    std::string line;
    std::getline(infile, line); // Skip header line
    std::vector<std::string_view> fields;
    fields.reserve(6);
    while (std::getline(infile, line))
    {

      fields = schedule::gtfs::utils::splitLineAndRemoveQuotes(line);
      if (fields.size() < 6)
      {
        continue;
      }

      aReader.getData().get().stops.emplace_back(std::string(fields[0]),
        std::string(fields[1]),
        geometry::Coordinate<double>(std::stod(std::string(fields[2]))),
        geometry::Coordinate<double>(std::stod(std::string(fields[3]))), std::string(fields[5]));
    }
  }

  GtfsStopReader::GtfsStopReader(std::string filename)
    : filename(std::move(filename)) {
  }
} // gtfs
