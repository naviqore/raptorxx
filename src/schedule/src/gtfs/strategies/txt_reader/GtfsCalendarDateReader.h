//
// Created by MichaelBrunner on 06/06/2024.
//

#ifndef GTFSCALENDARDATEREADER_H
#define GTFSCALENDARDATEREADER_H

#include <GtfsReader.h>
#include <string>

namespace schedule::gtfs {

  class GtfsCalendarDateReader
  {
    std::string filename;

  public:
    explicit GtfsCalendarDateReader(std::string filename);

    void operator()(GtfsReader& aReader) const;
  };

} // gtfs

#endif //GTFSCALENDARDATEREADER_H
