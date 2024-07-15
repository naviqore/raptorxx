//
// Created by MichaelBrunner on 13/07/2024.
//

#pragma once

#include "LoggerBridge.h"
#include <logging_export.h>

#include <memory>

enum class LOGGER_API LoggerName
{
  SCHEDULE,
  GTFS,
  RAPTOR,
  GEOMETRY,
};

enum class LOGGER_API Target
{
  CONSOLE,
  FILE
};

LOGGER_API std::shared_ptr<LoggerBridge> getLogger(Target target, LoggerName name);
