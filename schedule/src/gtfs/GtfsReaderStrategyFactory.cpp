//
// Created by MichaelBrunner on 15/07/2024.
//

#include "GtfsCsvReaderStrategyFactory.h"
#include "GtfsTxtReaderStrategyFactory.h"


#include <GtfsReaderStrategyFactory.h>


namespace schedule::gtfs {
  std::unique_ptr<IGtfsReaderStrategyFactory> createGtfsReaderStrategyFactory(const ReaderType type, std::string&& aGtfsFileDirectory) {
    if (type == ReaderType::CSV)
    {
      return std::make_unique<GtfsCsvReaderStrategyFactory>(std::move(aGtfsFileDirectory));
    }
    if (type == ReaderType::TXT)
    {
      return std::make_unique<GtfsTxtReaderStrategyFactory>(std::move(aGtfsFileDirectory));
    }
    throw std::runtime_error("Unknown reader type");
  }
} // gtfs
  // schedule