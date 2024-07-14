//
// Created by MichaelBrunner on 14/07/2024.
//

#ifndef GTFSAGENCYREADERCSV_H
#define GTFSAGENCYREADERCSV_H

#include "gtfs/GtfsReader.h"

#include <schedule_export.h>
#include <string>

namespace schedule::gtfs {

  class SCHEDULE_API GtfsAgencyReaderCsv
  {
    std::string filename;

  public:
    explicit GtfsAgencyReaderCsv(std::string&& filename);

    void operator()(GtfsReader& aReader) const;
  };


} // gtfs
// schedule

#endif //GTFSAGENCYREADERCSV_H
