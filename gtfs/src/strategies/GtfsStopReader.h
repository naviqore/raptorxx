//
// Created by MichaelBrunner on 07/06/2024.
//

#ifndef GTFSSTOPREADER_H
#define GTFSSTOPREADER_H

#include "GtfsReader.h"


#include <gtfs_export.h>
#include <string>

namespace gtfs {

  class GTFS_API GtfsStopReader
  {
    std::string filename;

  public:
    explicit GtfsStopReader(std::string filename);
    void operator()(GtfsReader& aReader) const;
  };

} // gtfs

#endif //GTFSSTOPREADER_H

