//
// Created by MichaelBrunner on 01/06/2024.
//

#ifndef GTFSAGENCYREADERSTRATEGY_H
#define GTFSAGENCYREADERSTRATEGY_H
#include "DataReader.h"
#include "fields/Agency.h"


#include <string_view>
#include <vector>


namespace gtfs {

  class GtfsReader;

  class GtfsAgencyReader
  {
  public:
    GtfsAgencyReader() = default;

    void operator()(GtfsReader& aReader) const;
  };

} // gtfs

#endif //GTFSAGENCYREADERSTRATEGY_H
