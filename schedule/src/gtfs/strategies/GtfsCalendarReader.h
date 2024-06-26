//
// Created by MichaelBrunner on 02/06/2024.
//

#ifndef GTFSCALENDARREADER_H
#define GTFSCALENDARREADER_H

#include "gtfs/GtfsReader.h"
#include <schedule_export.h>
#include <string>



namespace gtfs {

  class GTFS_API GtfsCalendarReader
  {
    std::string filename;

  public:
    explicit GtfsCalendarReader(std::string filename);

    void operator()(GtfsReader& aReader) const;
  };

} // gtfs

#endif //GTFSCALENDARREADER_H
