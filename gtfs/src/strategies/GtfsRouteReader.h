//
// Created by MichaelBrunner on 07/06/2024.
//

#ifndef GTFSROUTERREADER_H
#define GTFSROUTERREADER_H

#include <gtfs_export.h>
#include <string>

namespace gtfs {
  class GtfsReader;

  class GTFS_API GtfsRouteReader
  {
    std::string filename;

  public:
    explicit GtfsRouteReader(std::string filename);
    void operator()(GtfsReader& aReader) const;
  };

} // gtfs

#endif //GTFSROUTERREADER_H
