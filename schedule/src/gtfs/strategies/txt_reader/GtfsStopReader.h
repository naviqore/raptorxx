//
// Created by MichaelBrunner on 07/06/2024.
//

#ifndef GTFSSTOPREADER_H
#define GTFSSTOPREADER_H

#include <GtfsReader.h>
#include <string>

namespace schedule::gtfs {

  class GtfsStopReader
  {
    std::string filename;

  public:
    explicit GtfsStopReader(std::string filename);
    void operator()(GtfsReader& aReader) const;
  };

} // gtfs

#endif //GTFSSTOPREADER_H

