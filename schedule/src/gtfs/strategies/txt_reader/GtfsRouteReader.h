//
// Created by MichaelBrunner on 07/06/2024.
//

#ifndef GTFSROUTERREADER_H
#define GTFSROUTERREADER_H

#include <GtfsReader.h>
#include <string>

namespace schedule::gtfs {

  class GtfsRouteReader
  {
    std::string filename;

  public:
    explicit GtfsRouteReader(std::string filename);
    void operator()(GtfsReader& aReader) const;
  };

} // gtfs

#endif //GTFSROUTERREADER_H
