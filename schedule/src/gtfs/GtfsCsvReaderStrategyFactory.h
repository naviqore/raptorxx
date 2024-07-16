//
// Created by MichaelBrunner on 15/07/2024.
//

#pragma once

#include <GtfsReader.h>
#include "IGtfsReaderStrategyFactory.h"

#include <functional>
#include <string>
#include <unordered_map>

namespace schedule::gtfs {
  class GtfsCsvReaderStrategyFactory final : public IGtfsReaderStrategyFactory
  {
    std::string fileDirectory;

  public:

    explicit GtfsCsvReaderStrategyFactory(std::string&& aGtfsFileDirectory);

    std::function<void(GtfsReader&)>& getStrategy(GtfsStrategyType aType) override;

  private:
    std::map<utils::GTFS_FILE_TYPE, std::string> fileNameMap;

    std::unordered_map<GtfsStrategyType, std::function<void(GtfsReader&)>> strategies;

    void init();

    void setUpFileNameMap();
  };

} // gtfs  // schedule
