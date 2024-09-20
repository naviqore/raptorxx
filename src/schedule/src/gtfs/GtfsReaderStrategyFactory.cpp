//
// Created by MichaelBrunner on 15/07/2024.
//

#include "GtfsCsvParserReaderStrategyFactory.h"
#include "GtfsCsvReaderStrategyFactory.h"
#include "GtfsTxtReaderStrategyFactory.h"

#include <GtfsReaderStrategyFactory.h>


namespace schedule::gtfs {
  // Reason for using different strategies is to have a more flexible way to read the data from the GTFS files
  // The quality differs from framework to framework and from file to file for example UTF-8 BOM is handled by https://github.com/ben-strasser/fast-cpp-csv-parser already
  std::unique_ptr<IGtfsReaderStrategyFactory> createGtfsReaderStrategyFactory(const ReaderType type, std::string aGtfsFileDirectory) {
    if (type == ReaderType::CSV_PARALLEL)
    {
      return std::make_unique<GtfsCsvParserReaderStrategyFactory>(std::move(aGtfsFileDirectory));
    }
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