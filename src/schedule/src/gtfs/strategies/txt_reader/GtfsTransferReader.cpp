//
// Created by MichaelBrunner on 07/06/2024.
//

#include "GtfsTransferReader.h"


#include "LoggerFactory.h"
#include "src/utils/utils.h"

#include <stdexcept>
#include <fstream>
#include "GtfsReader.h"
#include "utils/scopedTimer.h"

namespace schedule::gtfs {
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

      aReader.getData().get().transfers[std::string(fields[headerMap["from_stop_id"]])].emplace_back(
        std::string(fields[headerMap["from_stop_id"]]),
        std::string(fields[headerMap["to_stop_id"]]),
        static_cast<Transfer::TransferType>(std::stoi(std::string(fields[headerMap["transfer_type"]]))),
        std::stoi(std::string(fields[headerMap["min_transfer_time"]])));

    }
  }
} // gtfs