//
// Created by MichaelBrunner on 28/05/2024.
//

#ifndef GTFSREADERSTRATEGY_H
#define GTFSREADERSTRATEGY_H

namespace gtfs {

  class GtfsReader;

  class GtfsReaderStrategy
  {
  public:
    explicit GtfsReaderStrategy() = default;

    void operator()(GtfsReader const& aReader) const;
  };

} // gtfs

#endif //GTFSREADERSTRATEGY_H
