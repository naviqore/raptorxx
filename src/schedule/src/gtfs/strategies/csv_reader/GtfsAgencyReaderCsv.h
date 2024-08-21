//
// Created by MichaelBrunner on 14/07/2024.
//

#ifndef GTFSAGENCYREADERCSV_H
#define GTFSAGENCYREADERCSV_H

#include <GtfsReader.h>
#include <string>

namespace schedule::gtfs {

  class GtfsAgencyReaderCsv
  {
    std::string filename;

  public:
    explicit GtfsAgencyReaderCsv(std::string&& filename);
    explicit GtfsAgencyReaderCsv(std::string const& filename);

    void operator()(GtfsReader& aReader) const;
  };


} // gtfs
// schedule

#endif //GTFSAGENCYREADERCSV_H
