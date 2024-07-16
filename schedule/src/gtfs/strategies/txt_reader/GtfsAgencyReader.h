//
// Created by MichaelBrunner on 01/06/2024.
//

#ifndef GTFSAGENCYREADERSTRATEGY_H
#define GTFSAGENCYREADERSTRATEGY_H


#include <GtfsReader.h>
#include <string>

namespace schedule::gtfs {

  class GtfsAgencyReader
  {
    std::string filename;

  public:
    explicit GtfsAgencyReader(std::string filename);

    void operator()(GtfsReader& aReader) const;
  };

} // gtfs

#endif //GTFSAGENCYREADERSTRATEGY_H
