//
// Created by MichaelBrunner on 23/06/2024.
//

#ifndef GTFSREADERSTRATEGYFACTORY_H
#define GTFSREADERSTRATEGYFACTORY_H
#include "GtfsReader.h"


#include <functional>
#include <string>
#include <unordered_map>


namespace schedule::gtfs {

  class GtfsReaderStrategyFactory
  {
  public:
    enum Type
    {
      AGENCY
    };
    GtfsReaderStrategyFactory();

    std::function<void(::gtfs::GtfsReader&)>& getStrategy(Type aType);

  private:
    std::unordered_map<Type, std::function<void(::gtfs::GtfsReader&)>> strategies;

    void init();
  };

} // gtfs
// schedule

#endif //GTFSREADERSTRATEGYFACTORY_H
