//
// Created by MichaelBrunner on 14/07/2024.
//

#pragma once

#include <GtfsReader.h>
#include <string>


namespace schedule::gtfs {

  class GtfsCalendarDateReaderCsvParser
  {
    std::string filename;

  public:
    explicit GtfsCalendarDateReaderCsvParser(std::string&& filename);
    explicit GtfsCalendarDateReaderCsvParser(std::string const& filename);

    void operator()(GtfsReader& aReader) const;
  };

} // gtfs
// schedule

