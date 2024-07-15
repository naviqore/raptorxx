//
// Created by MichaelBrunner on 15/07/2024.
//

#ifndef GTFSROUTEREADERCSV_H
#define GTFSROUTEREADERCSV_H

#include <string>
#include <GtfsReader.h>

namespace schedule::gtfs {


  class GtfsRouteReaderCsv
  {
    std::string filename;

  public:
    explicit GtfsRouteReaderCsv(std::string&& filename);

    void operator()(GtfsReader& aReader) const;
  };

} // gtfs
// schedule

#endif //GTFSROUTEREADERCSV_H
