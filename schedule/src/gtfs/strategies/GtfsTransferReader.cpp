//
// Created by MichaelBrunner on 07/06/2024.
//

#include "GtfsTransferReader.h"

#include "src/utils/utils.h"

#include <stdexcept>
#include <fstream>
#include "gtfs/GtfsReader.h"
#include "utils/scopedTimer.h"

namespace gtfs {
  GtfsTransferReader::GtfsTransferReader(std::string filename)
    : filename(std::move(filename)) {
  }
  void GtfsTransferReader::operator()(GtfsReader& aReader) const {
    MEASURE_FUNCTION(std::source_location().file_name());
    std::ifstream infile(filename);
    if (!infile.is_open())
    {
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

      aReader.getData().get().transfers.emplace_back(std::string(fields[0]),
                                                     std::string(fields[1]),
                                                     static_cast<schedule::gtfs::Transfer::TransferType>(std::stoi(std::string(fields[2]))),
                                                     std::stoi(std::string(fields[3])));
    }
  }
} // gtfs