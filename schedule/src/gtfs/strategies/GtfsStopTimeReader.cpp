//
// Created by MichaelBrunner on 07/06/2024.
//

#include "GtfsStopTimeReader.h"

#include "gtfs/GtfsReader.h"
#include "src/utils/utils.h"
#include "utils/scopedTimer.h"

#include <fstream>
#include <stdexcept>


namespace gtfs {

  void GtfsStopTimeReader::operator()(GtfsReader& aReader) const {
    using namespace std::string_literals;
    MEASURE_FUNCTION();

    std::ifstream infile(filename, std::ios::in | std::ios::binary);
    if (!infile.is_open())
    {
      throw std::runtime_error("Error opening file: " + std::string(filename));
    }

    constexpr size_t bufferSize = 1 << 22; // ~4MB
    std::vector<char> buffer(bufferSize);
    infile.rdbuf()->pubsetbuf(buffer.data(), bufferSize);

    std::string line;
    std::getline(infile, line); // Skip header line

    constexpr size_t expectedSizec = 16'891'069; // TODO this is a guess
    aReader.getData().get().stopTimes.reserve(expectedSizec);

    while (std::getline(infile, line))
    {
      auto fields = utils::splitLineAndRemoveQuotes(line);
      if (fields.size() < 7) {
        // TODO: Handle error
        continue;
      }

      aReader.getData().get().stopTimes.emplace_back(
          std::string(fields[0]), std::string(fields[1]),
          std::string(fields[2]), std::string(fields[3]),
          std::stoi(std::string(fields[4]))
      );
    }
  }


  GtfsStopTimeReader::GtfsStopTimeReader(std::string filename)
    : filename(std::move(filename)) {
  }
} // gtfs