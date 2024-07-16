//
// Created by MichaelBrunner on 15/07/2024.
//

#pragma once
#include <string>
#include <GtfsReader.h>

namespace schedule::gtfs {


  class GtfsRouteReaderCsvParser
  {
    std::string filename;

  public:
    explicit GtfsRouteReaderCsvParser(std::string&& filename);
    explicit GtfsRouteReaderCsvParser(std::string const& filename);

    void operator()(GtfsReader& aReader) const;
  };

} // gtfs
// schedule


