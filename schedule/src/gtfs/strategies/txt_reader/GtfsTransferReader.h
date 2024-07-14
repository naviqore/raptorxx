//
// Created by MichaelBrunner on 07/06/2024.
//

#ifndef GTFSTRANSFERREADER_H
#define GTFSTRANSFERREADER_H

#include "gtfs/GtfsReader.h"


#include <schedule_export.h>
#include <string>

namespace schedule::gtfs {

  class SCHEDULE_API GtfsTransferReader
  {
    std::string filename;

  public:
    explicit GtfsTransferReader(std::string filename);
    void operator()(GtfsReader& aReader) const;
  };

} // gtfs

#endif //GTFSTRANSFERREADER_H
