//
// Created by MichaelBrunner on 07/06/2024.
//

#ifndef GTFSSTOPTIMEREADER_H
#define GTFSSTOPTIMEREADER_H

#include <GtfsReader.h>
#include <string>

namespace schedule::gtfs {

  class GtfsStopTimeReader
  {
    std::string filename;

  public:
    explicit GtfsStopTimeReader(std::string filename);
    void operator()(GtfsReader& aReader) const;
  };

} // gtfs

#endif //GTFSSTOPTIMEREADER_H






