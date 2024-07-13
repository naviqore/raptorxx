//
// Created by MichaelBrunner on 07/06/2024.
//

#include "GtfsStopTimeReader.h"

#include "LoggerFactory.h"
#include "gtfs/GtfsReader.h"
#include "src/utils/utils.h"
#include "utils/scopedTimer.h"

#include <fstream>
#include <stdexcept>


namespace schedule::gtfs {
  void GtfsStopTimeReader::operator()(GtfsReader& aReader) const {
    using namespace std::string_literals;
    MEASURE_FUNCTION(std::source_location().file_name());

    std::ifstream infile(filename, std::ios::in | std::ios::binary);
    if (!infile.is_open())
    {
      throw std::runtime_error("Error opening file: " + std::string(filename));
    }
    getLogger(Target::CONSOLE, LoggerName::GTFS)->info(std::format("Reading file: {}", filename));
    constexpr size_t bufferSize = 1 << 22; // ~4MB
    std::vector<char> buffer(bufferSize);
    infile.rdbuf()->pubsetbuf(buffer.data(), bufferSize);

    std::string line;
    std::getline(infile, line); // Skip header line
    std::map<std::string, size_t> headerMap = utils::getGtfsColumnIndices(line);

    constexpr size_t expectedSizec = 16'891'069; // TODO this is a guess
    aReader.getData().get().stopTimes.reserve(expectedSizec);
    std::vector<std::string_view> fields;
    fields.reserve(7);

    while (std::getline(infile, line))
    {
      fields = utils::splitLineAndRemoveQuotes(line);
      if (fields.size() < 5)
      {
        // TODO: Handle error
        continue;
      }
      aReader.getData().get().stopTimes[std::string(fields[headerMap["stop_id"]])].emplace_back(
        std::string(fields[headerMap["trip_id"]]),
        std::string(fields[headerMap["arrival_time"]]),
        std::string(fields[headerMap["departure_time"]]),
        std::string(fields[headerMap["stop_id"]]),
        std::stoi(std::string(fields[headerMap["stop_sequence"]])));
    }
  }

  GtfsStopTimeReader::GtfsStopTimeReader(std::string filename)
    : filename(std::move(filename)) {
  }
} // gtfs
