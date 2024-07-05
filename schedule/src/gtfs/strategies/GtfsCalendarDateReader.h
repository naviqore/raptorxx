//
// Created by MichaelBrunner on 06/06/2024.
//

#ifndef GTFSCALENDARDATEREADER_H
#define GTFSCALENDARDATEREADER_H

#include "gtfs/GtfsReader.h"


#include <string>
#include <schedule_export.h>

namespace schedule::gtfs {

  class SCHEDULE_API GtfsCalendarDateReader
  {
    std::string filename;

  public:
    explicit GtfsCalendarDateReader(std::string filename);

    void operator()(GtfsReader& aReader) const;
  };

} // gtfs

#endif //GTFSCALENDARDATEREADER_H
