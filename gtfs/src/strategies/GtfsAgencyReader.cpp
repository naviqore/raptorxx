//
// Created by MichaelBrunner on 01/06/2024.
//

#include "GtfsAgencyReader.h"

#include "LoggingPool.h"
#include "src/GtfsReader.h"
#include "src/utils/utils.h"


#include <stdexcept>
#include <fstream>
#include <ranges>
#include <source_location>
#include <string>
#include <utility>

namespace gtfs {

  void GtfsAgencyReader::operator()(GtfsReader& aReader) const {
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
      if (fields.size() < 4)
      {
        // throw std::runtime_error("Error: insufficient number of fields. " + std::string(filename));
        // TODO: Handle error
        continue;
      }
      constexpr uint8_t quoteSize = 2; // this is needed to remove the quotes from the fields
      std::string id = fields[0].substr(1, fields[0].size() - quoteSize);
      std::string name = fields[1].substr(1, fields[1].size() - quoteSize);
      std::string timezone = fields[3].substr(1, fields[3].size() - quoteSize);

      aReader.getData().agencies.emplace_back(std::move(id), std::move(name), std::move(timezone));
    }
  }

  GtfsAgencyReader::GtfsAgencyReader(std::string filename)
    : filename(std::move(filename)) {
#ifdef NDEBUG
    // Release configuration.
#else
    // Debug configuration. NDEBUG is not defined.
    const auto threadId = std::this_thread::get_id();
    std::stringstream ss;
    ss << threadId;
    std::string threadIdString = ss.str();
    auto fileName = std::source_location::current().file_name();
    auto functionName = std::source_location::current().function_name();
    LoggingPool::getLogger()->info("{} line: {} GTFS data thread_id {}", fileName, functionName, threadIdString);
#endif
  }

} // gtfs
