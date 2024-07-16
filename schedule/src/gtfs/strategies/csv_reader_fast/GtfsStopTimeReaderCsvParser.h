//
// Created by MichaelBrunner on 15/07/2024.
//

#pragma once
#include <string>
#include <GtfsReader.h>

namespace schedule::gtfs {


  class GtfsStopTimeReaderCsvParser
  {
    std::string filename;

  public:
    explicit GtfsStopTimeReaderCsvParser(std::string&& filename);
    explicit GtfsStopTimeReaderCsvParser(std::string const& filename);

    void operator()(GtfsReader& aReader) const;
  };

} // gtfs
// schedule

