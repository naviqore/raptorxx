//
// Created by MichaelBrunner on 07/06/2024.
//

#ifndef GTFSTRANSFERREADER_H
#define GTFSTRANSFERREADER_H

#include "GtfsReader.h"


#include <gtfs_export.h>
#include <string>

namespace gtfs {

  class GTFS_API GtfsTransferReader
  {
    std::string filename;

  public:
    explicit GtfsTransferReader(std::string filename);
    void operator()(GtfsReader& aReader) const;
  };

} // gtfs

#endif //GTFSTRANSFERREADER_H
