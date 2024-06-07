//
// Created by MichaelBrunner on 28/05/2024.
//

#ifndef DATAREADER_H
#define DATAREADER_H

#include <gtfs_export.h>

namespace gtfs {
  struct GtfsData;
}

class GTFS_API DataReader
{
public:
  virtual ~DataReader() = default;
  virtual void readData() = 0;
  [[nodiscard]] virtual gtfs::GtfsData const& getData() const = 0;
};

#endif //DATAREADER_H
