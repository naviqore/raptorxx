//
// Created by MichaelBrunner on 15/07/2024.
//

#ifndef GTFSSTOPREADERCSV_H
#define GTFSSTOPREADERCSV_H

#include <string>
#include <GtfsReader.h>
namespace schedule::gtfs {


  class GtfsStopReaderCsv
  {
    std::string filename;

  public:
    explicit GtfsStopReaderCsv(std::string&& filename);

    void operator()(GtfsReader& aReader) const;
  };

} // gtfs
// schedule

#endif //GTFSSTOPREADERCSV_H
