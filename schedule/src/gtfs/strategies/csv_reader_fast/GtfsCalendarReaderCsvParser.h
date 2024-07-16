//
// Created by MichaelBrunner on 15/07/2024.
//

#pragma once

#include <string>
#include <GtfsReader.h>

namespace schedule::gtfs {


  class GtfsCalendarReaderCsvParser
  {
    std::string filename;

  public:
    explicit GtfsCalendarReaderCsvParser(std::string&& filename);
    explicit GtfsCalendarReaderCsvParser(std::string const& filename);

    void operator()(GtfsReader& aReader) const;
  };

} // gtfs
// schedule


