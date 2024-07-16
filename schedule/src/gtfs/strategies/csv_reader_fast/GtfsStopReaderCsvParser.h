//
// Created by MichaelBrunner on 15/07/2024.
//

#pragma once

#include <string>
#include <GtfsReader.h>
namespace schedule::gtfs {


  class GtfsStopReaderCsvParser
  {
    std::string filename;

  public:
    explicit GtfsStopReaderCsvParser(std::string&& filename);
    explicit GtfsStopReaderCsvParser(std::string const& filename);

    void operator()(GtfsReader& aReader) const;
  };

} // gtfs
// schedule
