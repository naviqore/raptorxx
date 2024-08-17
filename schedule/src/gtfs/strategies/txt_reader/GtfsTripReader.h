//
// Created by MichaelBrunner on 07/06/2024.
//

#ifndef GTFSTRIPREADER_H
#define GTFSTRIPREADER_H

#include <GtfsReader.h>
#include <string>

namespace schedule::gtfs {

  class GtfsTripReader
  {
    std::string filename;

  public:
    explicit GtfsTripReader(std::string filename);
    void operator()(GtfsReader& aReader) const;
  };

} // gtfs

#endif //GTFSTRIPREADER_H
