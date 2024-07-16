//
// Created by MichaelBrunner on 01/06/2024.
//

#include "GtfsAgencyReader.h"

#include "LoggerFactory.h"
#include "GtfsReader.h"
#include "src/utils/utils.h"
#include "utils/scopedTimer.h"


#include <stdexcept>
#include <fstream>
#include <ranges>
#include <source_location>
#include <string>
#include <utility>

namespace schedule::gtfs {

  void GtfsAgencyReader::operator()(GtfsReader& aReader) const {
    MEASURE_FUNCTION(std::source_location().file_name());
    std::ifstream infile(filename);
    if (!infile.is_open())
    {
      // TODO log error
      throw std::runtime_error("Error opening file: " + std::string(filename));
    }
    getLogger(Target::CONSOLE, LoggerName::GTFS)->info(std::format("Reading file: {}", filename));
    std::string line;
    std::getline(infile, line); // Skip header line
    std::map<std::string, size_t> headerMap = utils::getGtfsColumnIndices(line);
    std::vector<std::string_view> fields;
    fields.reserve(4);
    while (std::getline(infile, line))
    {
      fields = utils::splitLineAndRemoveQuotes(line);
      if (fields.size() < 4)
      {
        // TODO: Handle error
        continue;
      }
      aReader.getData().get().agencies.emplace(std::string(fields[headerMap["agency_name"]]),
                                               Agency{std::string(fields[headerMap["agency_id"]]),
                                                      std::string(fields[headerMap["agency_name"]]),
                                                      std::string(fields[headerMap["agency_timezone"]])});
    }
  }

  GtfsAgencyReader::GtfsAgencyReader(std::string filename)
    : filename(std::move(filename)) {
  }

} // gtfs