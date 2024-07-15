//
// Created by MichaelBrunner on 23/06/2024.
//

#ifndef GTFSREADERSTRATEGYFACTORY_H
#define GTFSREADERSTRATEGYFACTORY_H

#include <GtfsReader.h>
#include "IGtfsReaderStrategyFactory.h"

#include <functional>
#include <string>
#include <unordered_map>
#include <schedule_export.h>

namespace schedule::gtfs {

  class SCHEDULE_API GtfsTxtReaderStrategyFactory final : public IGtfsReaderStrategyFactory
  {
    std::string fileDirectory;

  public:

    explicit GtfsTxtReaderStrategyFactory(std::string&& aGtfsFileDirectory);

    std::function<void(GtfsReader&)>& getStrategy(GtfsStrategyType aType) override;

  private:
    std::map<utils::GTFS_FILE_TYPE, std::string> fileNameMap;

    std::unordered_map<GtfsStrategyType, std::function<void(GtfsReader&)>> strategies;

    void init();

    void setUpFileNameMap();
  };

} // gtfs
// schedule

#endif //GTFSREADERSTRATEGYFACTORY_H
