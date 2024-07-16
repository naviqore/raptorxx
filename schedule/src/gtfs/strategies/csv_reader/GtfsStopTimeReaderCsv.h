//
// Created by MichaelBrunner on 15/07/2024.
//

#ifndef GTFSSTOPTIMEREADERCSV_H
#define GTFSSTOPTIMEREADERCSV_H

#include <string>
#include <GtfsReader.h>

namespace schedule::gtfs {


  class GtfsStopTimeReaderCsv
  {
    std::string filename;

  public:
    explicit GtfsStopTimeReaderCsv(std::string&& filename);
    explicit GtfsStopTimeReaderCsv(std::string const& filename);

    void operator()(GtfsReader& aReader) const;
  };

} // gtfs
// schedule

#endif //GTFSSTOPTIMEREADERCSV_H
