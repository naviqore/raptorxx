//
// Created by MichaelBrunner on 28/05/2024.
//

#ifndef DATAREADER_H
#define DATAREADER_H

#include <schedule_export.h>

namespace schedule {
  template<typename T>
  class SCHEDULE_API DataReader
  {
  public:
    virtual ~DataReader() = default;
    virtual void readData() = 0;
    virtual T const& getData() const = 0;
    virtual T& getData() = 0;
  };
}

#endif //DATAREADER_H
