//
// Created by MichaelBrunner on 14/07/2024.
//

#ifndef GTFSCALENDARDATEREADERCSV_H
#define GTFSCALENDARDATEREADERCSV_H

#include <GtfsReader.h>
#include <string>


namespace schedule::gtfs {

  class GtfsCalendarDateReaderCsv
  {
    std::string filename;

  public:
    explicit GtfsCalendarDateReaderCsv(std::string&& filename);
    explicit GtfsCalendarDateReaderCsv(std::string const& filename);

    void operator()(GtfsReader& aReader) const;
  };

} // gtfs
// schedule

#endif //GTFSCALENDARDATEREADERCSV_H
