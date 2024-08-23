//
// Created by MichaelBrunner on 15/07/2024.
//

#pragma once

#include <string>
#include <GtfsReader.h>

namespace schedule::gtfs {

  class GtfsTripReaderCsvParser
  {
    std::string filename;

  public:
    explicit GtfsTripReaderCsvParser(std::string&& filename);
    explicit GtfsTripReaderCsvParser(std::string const& filename);

    void operator()(GtfsReader& aReader) const;
  };

} // gtfs // schedule
