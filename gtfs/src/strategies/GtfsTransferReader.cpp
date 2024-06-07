//
// Created by MichaelBrunner on 07/06/2024.
//

#include "GtfsTransferReader.h"

#include "src/utils/utils.h"

#include <stdexcept>
#include <fstream>
#include "src/GtfsReader.h"

namespace gtfs {
  GtfsTransferReader::GtfsTransferReader(std::string filename)
    : filename(std::move(filename)) {
  }
  void GtfsTransferReader::operator()(GtfsReader& aReader) const {
    std::ifstream infile(filename);
    if (!infile.is_open())
    {
      throw std::runtime_error("Error opening file: " + std::string(filename));
    }

    std::string line;
    std::getline(infile, line); // Skip header line

    while (std::getline(infile, line))
    {
      auto fields = utils::splitLine(line);
      if (constexpr uint8_t expectedNumberOfFields = 4; fields.size() != expectedNumberOfFields)
      {
        continue;
      }

      constexpr uint8_t quoteSize = 2; // this is needed to remove the quotes from the fields
      std::string fromStopId = fields[0].substr(1, fields[0].size() - quoteSize);
      std::string toStopId = fields[1].substr(1, fields[1].size() - quoteSize);
      const int transferType = std::stoi(fields[2].substr(1, fields[2].size() - quoteSize));

      aReader.getData().transfers.emplace_back(std::move(fromStopId), std::move(toStopId), transferType);
    }
  }
} // gtfs