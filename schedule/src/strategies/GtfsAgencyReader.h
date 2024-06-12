//
// Created by MichaelBrunner on 01/06/2024.
//

#ifndef GTFSAGENCYREADERSTRATEGY_H
#define GTFSAGENCYREADERSTRATEGY_H


#include "GtfsCalendarDateReader.h"
#include "GtfsData.h"


#include <schedule_export.h>
#include <string>

namespace gtfs {

  class GTFS_API GtfsAgencyReader
  {
    std::string filename;

  public:
    explicit GtfsAgencyReader(std::string filename);

    void operator()(GtfsReader& aReader) const;
  };

} // gtfs

#endif //GTFSAGENCYREADERSTRATEGY_H
