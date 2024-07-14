//
// Created by MichaelBrunner on 14/07/2024.
//

#ifndef GTFSCALENDARDATEREADERCSV_H
#define GTFSCALENDARDATEREADERCSV_H

#include "gtfs/GtfsReader.h"


#include <string>
#include <schedule_export.h>


namespace schedule::gtfs {

  class SCHEDULE_API GtfsCalendarDateReaderCsv
  {
    std::string filename;

  public:
    explicit GtfsCalendarDateReaderCsv(std::string&& filename);

    void operator()(GtfsReader& aReader) const;
  };

} // gtfs
// schedule

#endif //GTFSCALENDARDATEREADERCSV_H
