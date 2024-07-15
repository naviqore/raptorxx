//
// Created by MichaelBrunner on 15/07/2024.
//

#ifndef GTFSTRIPREADERCSV_H
#define GTFSTRIPREADERCSV_H

#include <string>
#include <GtfsReader.h>

namespace schedule::gtfs {

  class GtfsTripReaderCsv
  {
    std::string filename;

  public:
    explicit GtfsTripReaderCsv(std::string&& filename);

    void operator()(GtfsReader& aReader) const;
  };

} // gtfs // schedule

#endif //GTFSTRIPREADERCSV_H
