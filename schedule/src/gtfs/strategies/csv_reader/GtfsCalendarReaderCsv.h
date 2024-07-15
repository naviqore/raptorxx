//
// Created by MichaelBrunner on 15/07/2024.
//

#ifndef GTFSCALENDARREADERCSV_H
#define GTFSCALENDARREADERCSV_H

#include <string>
#include <GtfsReader.h>

namespace schedule::gtfs {


  class GtfsCalendarReaderCsv
  {
    std::string filename;

  public:
    explicit GtfsCalendarReaderCsv(std::string&& filename);

    void operator()(GtfsReader& aReader) const;
  };

} // gtfs
// schedule

#endif //GTFSCALENDARREADERCSV_H
