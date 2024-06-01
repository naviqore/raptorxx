//
// Created by MichaelBrunner on 01/06/2024.
//

#include "GtfsAgencyReader.h"

#include "GtfsReader.h"
#include "utils/utils.h"

#include <stdexcept>
#include <fstream>
#include <ranges>

namespace gtfs {

  void GtfsAgencyReader::operator()(GtfsReader& aReader) const {
    std::ifstream infile(aReader.getFilename().data());
    if (!infile.is_open())
    {
      // TODO log error
      throw std::runtime_error("Error opening file: " + std::string(aReader.getFilename()));
    }

    std::string line;
    std::getline(infile, line); // Skip header line

    while (std::getline(infile, line))
    {
      auto fields = split_line(line);
      if (fields.size() < 4)
      {
        // TODO: Handle error
        continue;
      }
      constexpr uint8_t quoteSize = 2; // this is needed to remove the quotes from the fields
      std::string id = fields[0].substr(1, fields[0].size() - quoteSize);
      std::string name = fields[1].substr(1, fields[1].size() - quoteSize);
      std::string timezone = fields[3].substr(1, fields[3].size() - quoteSize);

      aReader.agencies.emplace_back(std::move(id), std::move(name), std::move(timezone));
    }
  }
  std::vector<Agency> const& GtfsAgencyReader::getAgencies() const {
    {
      return agencies;
    }
  }
} // gtfs
