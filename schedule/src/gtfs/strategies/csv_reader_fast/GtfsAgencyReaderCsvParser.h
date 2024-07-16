//
// Created by MichaelBrunner on 14/07/2024.
//

#pragma once

#include <GtfsReader.h>
#include <string>

namespace schedule::gtfs {

  class GtfsAgencyReaderCsvParser
  {
    std::string filename;

  public:
    explicit GtfsAgencyReaderCsvParser(std::string&& filename);
    explicit GtfsAgencyReaderCsvParser(std::string const& filename);

    void operator()(GtfsReader& aReader) const;
  };


} // gtfs
// schedule


