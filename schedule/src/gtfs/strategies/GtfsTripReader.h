//
// Created by MichaelBrunner on 07/06/2024.
//

#ifndef GTFSTRIPREADER_H
#define GTFSTRIPREADER_H

#include "gtfs/GtfsReader.h"

#include <schedule_export.h>
#include <string>

namespace schedule::gtfs {

  class SCHEDULE_API GtfsTripReader
  {
    std::string filename;

  public:
    explicit GtfsTripReader(std::string filename);
    void operator()(GtfsReader& aReader) const;
  };

} // gtfs

#endif //GTFSTRIPREADER_H
