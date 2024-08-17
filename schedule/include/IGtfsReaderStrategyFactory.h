//
// Created by MichaelBrunner on 15/07/2024.
//

#ifndef IGTFSREADERSTRATEGYFACTORY_H
#define IGTFSREADERSTRATEGYFACTORY_H

#include <schedule_export.h>

#include <functional>

enum class GtfsStrategyType
{
  AGENCY,
  CALENDAR,
  CALENDAR_DATE,
  ROUTE,
  STOP,
  STOP_TIME,
  TRANSFER,
  TRIP
};


namespace schedule::gtfs {

  class GtfsReader;

  class SCHEDULE_API IGtfsReaderStrategyFactory
  {
  public:
    virtual ~IGtfsReaderStrategyFactory() = default;

    virtual std::function<void(GtfsReader&)>& getStrategy(GtfsStrategyType aType) = 0;
  };

} // gtfs

#endif //IGTFSREADERSTRATEGYFACTORY_H
