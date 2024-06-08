//
// Created by MichaelBrunner on 06/06/2024.
//

#ifndef GTFSCALENDARDATEREADER_H
#define GTFSCALENDARDATEREADER_H

#include "GtfsReader.h"


#include <string>
#include <gtfs_export.h>

namespace gtfs {

  class GTFS_API GtfsCalendarDateReader
  {
    std::string filename;

  public:
    explicit GtfsCalendarDateReader(std::string filename);

    void operator()(GtfsReader& aReader) const;
  };

} // gtfs

#endif //GTFSCALENDARDATEREADER_H
