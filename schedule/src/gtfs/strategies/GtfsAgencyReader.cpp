//
// Created by MichaelBrunner on 01/06/2024.
//

#include "GtfsAgencyReader.h"

#include "LoggingPool.h"
#include "gtfs/GtfsReader.h"
#include "src/utils/utils.h"
#include "utils/scopedTimer.h"


#include <stdexcept>
#include <fstream>
#include <ranges>
#include <source_location>
#include <string>
#include <utility>

namespace gtfs {

  void GtfsAgencyReader::operator()(GtfsReader& aReader) const {
    MEASURE_FUNCTION(std::source_location().file_name());
    std::ifstream infile(filename);
    if (!infile.is_open())
    {
      // TODO log error
      throw std::runtime_error("Error opening file: " + std::string(filename));
    }

    std::string line;
    std::getline(infile, line); // Skip header line
    std::vector<std::string_view> fields;
    fields.reserve(4);
    while (std::getline(infile, line))
    {
      fields = schedule::gtfs::utils::splitLineAndRemoveQuotes(line);
      if (fields.size() < 4)
      {
        // TODO: Handle error
        continue;
      }
      aReader.getData().get().agencies.emplace_back(std::string(fields[0]), std::string(fields[1]),
                                                    std::string(fields[3]));
    }
  }

  GtfsAgencyReader::GtfsAgencyReader(std::string filename)
    : filename(std::move(filename)) {
  }

} // gtfs
