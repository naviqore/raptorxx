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
    while (std::getline(infile, line))
    {
      auto fields = utils::splitLine(line);
      if (constexpr uint8_t expectedNumberOfFields = 4; fields.size() < expectedNumberOfFields)
      {
        // throw std::runtime_error("Error: insufficient number of fields. " + std::string(filename));
        // TODO: Handle error
        continue;
      }
      constexpr uint8_t quoteSize = 2; // this is needed to remove the quotes from the fields
      std::string id = fields[0].substr(1, fields[0].size() - quoteSize);
      std::string name = fields[1].substr(1, fields[1].size() - quoteSize);
      std::string timezone = fields[3].substr(1, fields[3].size() - quoteSize);

      aReader.getData().get().agencies.emplace_back(std::move(id), std::move(name), std::move(timezone));
    }
  }

  GtfsAgencyReader::GtfsAgencyReader(std::string filename)
    : filename(std::move(filename)) {
  }

} // gtfs
