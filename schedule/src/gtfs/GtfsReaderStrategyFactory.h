//
// Created by MichaelBrunner on 23/06/2024.
//

#ifndef GTFSREADERSTRATEGYFACTORY_H
#define GTFSREADERSTRATEGYFACTORY_H

#include "GtfsReader.h"

#include <functional>
#include <string>
#include <unordered_map>
#include <schedule_export.h>

namespace schedule::gtfs {

  class SCHEDULE_API GtfsReaderStrategyFactory
  {
    std::string fileDirectory;

  public:
    enum Type
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
    explicit GtfsReaderStrategyFactory(std::string&& aGtfsFileDirectory);

    std::function<void(GtfsReader&)>& getStrategy(Type aType);

  private:
    std::map<utils::GTFS_FILE_TYPE, std::string> lFileNameMap;

    std::unordered_map<Type, std::function<void(GtfsReader&)>> strategies;

    void init();

    void setUpFileNameMap();
  };

} // gtfs
// schedule

#endif //GTFSREADERSTRATEGYFACTORY_H
