//
// Created by MichaelBrunner on 01/06/2024.
//

#ifndef GTFSDATA_H
#define GTFSDATA_H
#include "fields/Agency.h"


#include <vector>

namespace gtfs {

  struct GtfsData
  {
    std::vector<Agency> agencies;
  };

} // gtfs

#endif //GTFSDATA_H
