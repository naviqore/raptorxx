//
// Created by MichaelBrunner on 01/06/2024.
//

#ifndef GTFSAGENCYREADERSTRATEGY_H
#define GTFSAGENCYREADERSTRATEGY_H
#include <string_view>


namespace gtfs {

  class GtfsReader;

  class GtfsAgencyReaderStrategy
  {

  public:
    explicit GtfsAgencyReaderStrategy(std::string_view filename);

    void operator()(GtfsReader const& aReader) const;

  private:
    std::string_view filename;
  };

} // gtfs

#endif //GTFSAGENCYREADERSTRATEGY_H
