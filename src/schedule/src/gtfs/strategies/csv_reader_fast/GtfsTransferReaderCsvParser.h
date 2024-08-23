//
// Created by MichaelBrunner on 15/07/2024.
//

#pragma once

#include <string>
#include <GtfsReader.h>

namespace schedule::gtfs {


  class GtfsTransferReaderCsvParser
  {
    std::string filename;

  public:
    explicit GtfsTransferReaderCsvParser(std::string&& filename);
    explicit GtfsTransferReaderCsvParser(std::string const& filename);

    void operator()(GtfsReader& aReader) const;
  }; // gtfs
} // schedule

