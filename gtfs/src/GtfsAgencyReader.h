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
    std::vector<Agency> agencies;

  public:
    GtfsAgencyReader() = default;

    void operator()(GtfsReader& aReader) const;

    [[nodiscard]] std::vector<Agency> const& getAgencies() const;
  };

} // gtfs

#endif //GTFSAGENCYREADERSTRATEGY_H
