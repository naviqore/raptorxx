//
// Created by MichaelBrunner on 07/06/2024.
//

#include "GtfsTransferReader.h"

#include "src/utils/utils.h"

#include <stdexcept>
#include <fstream>
#include "gtfs/GtfsReader.h"

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
      // const int transferType = std::stoi(fields[2].substr(1, fields[2].size() - quoteSize));

      Transfer::TransferType transferType;

      switch (fields[2][0])
      {
        case '0':
          transferType = Transfer::TransferType::RECOMMENDED;
          break;
        case '1':
          transferType = Transfer::TransferType::TIMED;
          break;
        case '2':
          transferType = Transfer::TransferType::MINIMUM_TIME;
          break;
        case '3':
          transferType = Transfer::TransferType::NOT_POSSIBLE;
          break;
        default:
          break;
          //throw std::runtime_error("Error: invalid transfer type.");
      }

      aReader.getData().get().transfers.emplace_back(std::move(fromStopId), std::move(toStopId), transferType);
    }
  }
} // gtfs