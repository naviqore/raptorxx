//
// Created by MichaelBrunner on 15/07/2024.
//

#pragma once

#include "IGtfsReaderStrategyFactory.h"
#include <string>
#include <memory>
#include <schedule_export.h>

namespace schedule::gtfs {

  enum class SCHEDULE_API ReaderType
  {
    TXT,
    CSV,
    CSV_PARALLEL
  };

  SCHEDULE_API std::unique_ptr<IGtfsReaderStrategyFactory> createGtfsReaderStrategyFactory(ReaderType type, std::string&& aGtfsFileDirectory);

} // gtfs
// schedule
