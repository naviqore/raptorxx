//
// Created by MichaelBrunner on 28/05/2024.
//

#ifndef DATAREADER_H
#define DATAREADER_H

#include <gtfs_export.h>

namespace gtfs {
  struct GtfsData;
}

template<typename T>
class GTFS_API DataReader
{
protected:
  T data;

  DataReader() = default;

public:
  virtual ~DataReader() = default;
  virtual void readData() = 0;

  [[nodiscard]] T const& getData() const {
    return data;
  }

  [[nodiscard]] T& getData() {
    return data;
  }
};

#endif //DATAREADER_H
