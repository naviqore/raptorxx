//
// Created by MichaelBrunner on 07/06/2024.
//

#ifndef GTFSSTOPTIMEREADER_H
#define GTFSSTOPTIMEREADER_H

#include <gtfs_export.h>
#include <string>

namespace gtfs {

  class GtfsReader;

  class GTFS_API GtfsStopTimeReader
  {
    std::string filename;

  public:
    explicit GtfsStopTimeReader(std::string filename);
    void operator()(GtfsReader& aReader) const;
  };

} // gtfs

#endif //GTFSSTOPTIMEREADER_H






