//
// Created by MichaelBrunner on 15/07/2024.
//

#ifndef GTFSTRANSFERREADERCSV_H
#define GTFSTRANSFERREADERCSV_H

#include <string>
#include <GtfsReader.h>

namespace schedule::gtfs {


  class GtfsTransferReaderCsv
  {
    std::string filename;

  public:
    explicit GtfsTransferReaderCsv(std::string&& filename);

    void operator()(GtfsReader& aReader) const;
  }; // gtfs
} // schedule

#endif //GTFSTRANSFERREADERCSV_H
